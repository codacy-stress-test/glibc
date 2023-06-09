#include <errno.h>
#include <libgen.h>
#undef basename
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  /* Make sure we do not find a binary with the name we are going to
     use.  */
  char *bindir = strdupa (copy);
  bindir = canonicalize_file_name (dirname (bindir));
  if (bindir == NULL)
    {
      puts ("canonicalize_file_name failed");
      return 1;
    }

  char *path = xasprintf ("%s:../libio:../elf", bindir);

  setenv ("PATH", path, 1);

  char *prog = basename (copy);
  errno = 0;
  execlp (prog, prog, NULL);

  if (errno != EACCES)
    {
      printf ("errno = %d (%m), expected EACCES\n", errno);
      return 1;
    }

  return 0;
}
