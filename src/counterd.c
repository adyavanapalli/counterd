// counterd.c

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

void create_new_session()
{
    if (setsid() == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }
}

void change_to_root_directory()
{
    if (chdir("/") == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }
}

void close_open_file_descriptors()
{
    long max_fd = 0;
    if ((max_fd = sysconf(_SC_OPEN_MAX)) == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }

    for (long fd = 0; fd < max_fd &&
                      // We do not close stderr just yet in case we need
                      // to print an error to it.
                      fd != stderr->_fileno; fd++)
    {
        if (close(fd) == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));

            exit(EXIT_FAILURE);
        }
    }

    // But now we close it.
    if (fclose(stderr) != 0)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }
}

void redirect_to_dev_null()
{
    if (open("/dev/null", O_RDWR) != STDIN_FILENO)
    {
        exit(EXIT_FAILURE);
    }

    if (open("/dev/null", O_RDWR) != STDOUT_FILENO)
    {
        exit(EXIT_FAILURE);
    }

    if (open("/dev/null", O_RDWR) != STDERR_FILENO)
    {
        exit(EXIT_FAILURE);
    }
}

void start_counting(int argc, char* argv[])
{
    if (argc != 1)
    {
        exit(EXIT_FAILURE);
    }

    openlog(argv[0], LOG_PID, LOG_LOCAL0);

    syslog(LOG_INFO | LOG_LOCAL0, "Starting count");

    for (size_t i = 1; i <= 100; i++)
    {
        syslog(LOG_INFO | LOG_LOCAL0, "count: %lu", i);
        sleep(1);
    }

    syslog(LOG_INFO | LOG_LOCAL0, "Ending count");

    closelog();
}

int main(int argc, char* argv[])
{
    pid_t fork_result = fork();
    if (fork_result > 0)
    {
        // This is the parent process.

        exit(EXIT_SUCCESS);
    }
    else if (fork_result == 0)
    {
        // This is the child process.

        // We call setsid to create a new session and free it of any association
        // with a controlling terminal.
        create_new_session();

        // We call fork again and continue with the grandchild, and since it is
        // not the session leader, it can no longer acquire a controlling
        // terminal.
        if ((fork_result = fork()) > 0)
        {
            // This is still the child process.
            
            exit(EXIT_SUCCESS);
        }
        else if (fork_result == 0)
        {
            // This is the grandchild process.

            // We clear the file mode creation mask so that when the daemon
            // creates any files, all users have access permissions to those
            // files. But why would other users need access permissions to these
            // files?
            umask(0);

            // Since the daemon is a long-running process, it will inevitably
            // live until system shutdown, but if it lives on a file system
            // other than root, that file system cannot be unmounted.
            change_to_root_directory();

            // Since the daemon no longer has the controlling terminal, it has
            // no use for the usual std* file descriptors and any other file
            // descriptors for that matter.
            close_open_file_descriptors();

            // And finally, we redirect all data written to std* to /dev/null so
            // that if the daemon processes any I/O on those descriptors, it
            // will not fail.
            redirect_to_dev_null();

            // And now begin the entire purpose of this daemon i.e. to count.
            start_counting(argc, argv);
            
            exit(EXIT_SUCCESS);
        }
        else if (fork_result == -1)
        {
            fprintf(stderr, "error: %s\n", strerror(errno));

            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
    else if (fork_result == -1)
    {
        fprintf(stderr, "error: %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }

    return 0;
}
