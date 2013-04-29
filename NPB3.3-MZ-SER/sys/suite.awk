BEGIN { SMAKE = "make"; printf "make header;" } {
  if ($1 !~ /^#/ &&  NF > 1) {
    printf "cd `echo %s|tr '[a-z]' '[A-Z]'`; %s clean;", $1, SMAKE;
    printf "%s -j8 CLASS=%s", SMAKE, $2;
    if (NF > 2) {
      printf " VERSION=%s", $3;
    }
    printf "; cd ..\n";
  }
}
