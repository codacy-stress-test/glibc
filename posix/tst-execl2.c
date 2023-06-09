#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>


static void prepare (int argc, char *argv[]);
static int do_test (void);
#define PREPARE(argc, argv) prepare (argc, argv)
#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"


static char *copy;

static void
prepare (int argc, char *argv[])
{
  char *buf;
  int off;

  buf = xasprintf ("cp %s %n%s-copy", argv[0], &off, argv[0]);
  if (system (buf) != 0)
    {
      puts ("system  failed");
      exit (1);
    }

  /* Make it not executable.  */
  copy = buf + off;
  if (chmod (copy, 0666) != 0)
    {
      puts ("chmod  failed");
      exit (1);
    }

  add_temp_file (copy);
}


static int
do_test (void)
{
  errno = 0;
  execl (copy, copy, NULL);

  if (errno != EACCES)
    {
      printf ("errno = %d (%m), expected EACCES\n", errno);
      return 1;
    }

  return 0;
}
