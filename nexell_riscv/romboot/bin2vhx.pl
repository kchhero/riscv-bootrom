#!/usr/bin/perl
use strict; use warnings;

	print "\n[DEBUG] pwd = ";
	system("pwd");

  # 읽기용 파일을, 이진 모드로 열기
  open(FH, "<$ARGV[0]") || die "can't open $ARGV[0]";
  printf($ARGV[0]);
  binmode FH;

  my($ss);
  my($print_retval);
	
  # 쓰기용 파일을, 텍스트 모드로 열기
  open(TEXT, ">$ARGV[1]") || die "can't open $ARGV[1]";
  printf($ARGV[1]);

  my $idx = 0;
  while (read FH, $_, 1) {
	  $ss = sprintf("%02X\n", ord($_));
	  #print "\n[DEBUG] ss = $ss (idx=$idx)";
	  $idx = $idx + 1;
      print TEXT  $ss; 

	  $print_retval = $?;
      if($print_retval != 0)
      {
      	print "ERROR on print to FILE..";
      	exit(-1);
      }
  }

  close FH;
  close TEXT;
