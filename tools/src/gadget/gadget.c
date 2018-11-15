/************************************************************************/
/* rop-tool - A Return Oriented Programming and binary exploitation     */
/*            tool                                                      */
/*                                                                      */
/* Copyright 2013-2015, -TOSH-                                          */
/* File coded by -TOSH-                                                 */
/*                                                                      */
/* This file is part of rop-tool.                                       */
/*                                                                      */
/* rop-tool is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* rop-tool is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with rop-tool.  If not, see <http://www.gnu.org/licenses/>     */
/************************************************************************/
#include "rop_gadget.h"

#define GADGET_DEFAULT_DEPTH 5
#define GADGET_MAX_DEPTH 50

u32 gadget_options_depth = GADGET_DEFAULT_DEPTH;
int gadget_options_filter = 1;
int gadget_options_color = 1;
int gadget_options_all = 0;
r_binfmt_arch_e gadget_options_arch = R_BINFMT_ARCH_UNDEF;
r_binfmt_endian_e gadget_options_endian = R_BINFMT_ENDIAN_UNDEF;
r_disa_flavor_e gadget_options_flavor = R_DISA_FLAVOR_UNDEF;
r_utils_bytes_s *gadget_options_bad = NULL;
const char *gadget_options_filename = "a.out";

void gadget_help(void) {
  printf("Usage : %s gadget [OPTIONS] [FILENAME]\n\n", PACKAGE);
  printf("OPTIONS:\n");
  printf("  --arch, -A               Select an architecture (use -A list to see available architectures)\n");
  printf("  --all, -a                Print all gadgets (even gadgets which are not uniq)\n");
  printf("  --bad, -B           [b]  Specify bad chars in address\n");
  printf("  --depth, -d         [d]  Specify the depth for gadget searching (default is %d)\n", GADGET_DEFAULT_DEPTH);
  printf("  --flavor, -f        [f]  Select a flavor (att or intel)\n");
  printf("  --no-filter, -F          Do not apply some filters on gadgets\n");
  printf("  --help, -h               Print this help message\n");
  printf("  --no-color, -N           Do not colorize output\n");
  printf("\n");
}


/* Parse command line options */
void gadget_options_parse(int argc, char **argv) {
  int opt;

  const struct option opts[] = {
    {"arch",          required_argument, NULL, 'A'},
    {"all",           no_argument,       NULL, 'a'},
    {"bad",           required_argument, NULL, 'B'},
    {"depth",         required_argument, NULL, 'd'},
    {"flavor",        required_argument, NULL, 'f'},
    {"no-filter",     no_argument,       NULL, 'F'},
    {"help",          no_argument,       NULL, 'h'},
    {"no-color",      no_argument,       NULL, 'N'},
    {NULL,            0,                 NULL, 0  }
  };

  while((opt = getopt_long(argc, argv, "A:ab:B:d:f:FhN", opts, NULL)) != -1) {
    switch(opt) {

    case 'A':
      if(!strcmp(optarg, "list")) {
        r_disa_list_architectures();
        exit(EXIT_SUCCESS);
      }

      if(!r_disa_string_to_arch(optarg, &gadget_options_arch, &gadget_options_endian))
        R_UTILS_ERR("%s: bad architecture.", optarg);
      break;

    case 'a':
      gadget_options_all = 1;
      break;


    case 'B':
      gadget_options_bad = r_utils_bytes_unhexlify(optarg);
      break;

    case 'd':
      gadget_options_depth = strtoull(optarg, NULL, 0);
      if(gadget_options_depth <= 0 || gadget_options_depth > GADGET_MAX_DEPTH)
        R_UTILS_ERR("%s: bad depth.", optarg);
      break;

    case 'f':
      gadget_options_flavor = r_disa_string_to_flavor(optarg);
      if(gadget_options_flavor == R_DISA_FLAVOR_UNDEF)
        R_UTILS_ERR("%s: bad flavor.", optarg);
      break;

    case 'F':
      gadget_options_filter = 0;
      break;

    case 'h':
      gadget_help();
      exit(EXIT_FAILURE);
      break;

    case 'N':
      gadget_options_color = 0;
      break;

    default:
      gadget_help();
      exit(EXIT_FAILURE);
    }
  }

  if(optind < argc) {
    gadget_options_filename = argv[optind];
  }
}

void gadget_cmd(int argc, char **argv) {
  r_binfmt_s bin;

  gadget_options_parse(argc, argv);

  r_binfmt_load(&bin, gadget_options_filename, gadget_options_arch, gadget_options_endian);
  R_UTILS_PRINT_YELLOW_BG_BLACK(gadget_options_color, "Looking gadgets, please wait...\n");
  gadget_print_search(&bin);
  r_binfmt_free(&bin);
}
