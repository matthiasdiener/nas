BEGIN { SMAKE = "make"; printf "make header;" } {
  if ($1 !~ /^#/ &&  NF > 2) {
    printf "cd `echo %s|tr '[a-z]' '[A-Z]'`; %s clean;", $1, SMAKE;
    printf "%s -j8 CLASS=%s NPROCS=%s", SMAKE, $2, $3;
    if ( NF > 3 ) {
      printf " VERSION=%s", $4;
    }
    printf "; cd ..\n";
  }
}
