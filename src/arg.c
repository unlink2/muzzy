#include "arg.h"
#include "libmuzzy/config.h"
#include "libmuzzy/log.h"
#include <argtable2.h>
#include <limits.h>

struct muzzy_config muzzy_args_to_config(int argc, char **argv) {
  struct arg_lit *verb = NULL;
  struct arg_lit *help = NULL;
  struct arg_lit *version = NULL;

  struct arg_file *words = NULL;
  struct arg_str *replace = NULL;
  struct arg_lit *no_sh = NULL;
  struct arg_int *delay_ms = NULL;
  struct arg_lit *stdrand = NULL;
  struct arg_int *seed_rand = NULL;
  struct arg_int *n_runs = NULL;
  struct arg_file *rand_file = NULL;

  struct arg_str *command = NULL;

  // arg end stores errors
  struct arg_end *end = NULL;

  void *argtable[] = {
      help = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
      version =
          arg_litn(NULL, "version", 0, 1, "display version info and exit"),
      verb = arg_litn("v", "verbose", 0, MUZZY_LOG_LEVEL_DBG, "verbose output"),

      words = arg_file0("w", "words", "word list", "Read a word list file"),
      replace = arg_str0(
          "r", "replace", "replace string",
          "Add a replace string. These are read in order for each word list."),

      no_sh =
          arg_lit0(NULL, "nosh",
                   "Execute command directly instead of through '/bin/sh -c'."),
      delay_ms = arg_int0(NULL, "delay", "MS", "Delay between attempts in ms."),
      stdrand = arg_lit0(NULL, "stdrand", "Use built-in rand instead."),
      seed_rand = arg_int0(NULL, "seed", "INT", "Seed the built-in rand."),
      n_runs = arg_int0("n", "nruns", "INT", "Amount of attempts to run"),
      rand_file = arg_file0(NULL, "frand", "FILE",
                            "The file to read from as a source of randomness. "
                            "Defaults to '/dev/urandom'."),

      command = arg_strn(NULL, NULL, "", 1, 4096, "The command to execute"),
      end = arg_end(20),
  };

  // output params
  char progname[] = "muzzy";
  char short_desc[] =
      "muzzy - mini fuzzer\nWritten by Lukas Krickl <lukas@krickl.dev>";

  // version info
  int version_major = 0;
  int version_minor = 0;
  int version_patch = 1;

  int nerrors = arg_parse(argc, argv, argtable);
  int exitcode = 0;

  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf("%s\n\n", short_desc);
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    goto exit;
  }

  if (version->count) {
    printf("%s version %d.%d.%d\n", progname, version_major, version_minor,
           version_patch);
    goto exit;
  }

  if (verb->count > 0) {
    muzzy_log_init(verb->count);
  } else {
    muzzy_log_init(MUZZY_LOG_LEVEL_ERROR);
  }

  if (nerrors > 0) {
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exit;
  }

  struct muzzy_config cfg = muzzy_config_init();

  // map args to cfg data here

  // map command to run here
  if (no_sh->count) {

  } else {
  }

  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return cfg;
exit:
  exit(exitcode); // NOLINT
}
