/*-----------------------------------------------------------------------
 *  PerlCore.cpp --
 *
 *  .
 *
 *-----------------------------------------------------------------------
 * RCS: @(#) $Id: PerlCore.cpp,v 1.4 2003/01/07 22:56:00 davygrvy Exp $
 *-----------------------------------------------------------------------
 */

#if IRCE_USEPERL

#include "PerlCore.hpp"

EXTERN_C void boot_DynaLoader (pTHXo_ CV* cv);

static void
#ifdef OLD_PERL
xs_init()
#else
xs_init(pTHXo)
#endif
{
    dXSUB_SYS;
    /* allow dynamic loading of binary modules */
    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
}

const char
Perl::Interp::perl_definitions[] =
{
  /* We use two function one to load a file the other to
     execute the string obtained from the first and holding
     the file conents. This allows to have a realy local $/
     without introducing temp variables to hold the old
     value. Just a question of style:) 
     We also redefine the $SIG{__WARN__} handler to have XChat
     printing warnings in the main window. (TheHobbit)*/
  "sub load_file{"
    "my $f_name=shift;"
    "local $/=undef;"
    "open FH,$f_name or return \"__FAILED__\";"
    "$_=<FH>;"
    "close FH;"
    "return $_;"
  "}"
  "sub load_n_eval{"
    "my $f_name=shift;"
    "my $strin=load_file($f_name);"
    "return 2 if($strin eq \"__FAILED__\");"
    "eval $strin;"
    "if($@){"
    /*"  #something went wrong\n"*/
      "IRC::echo\"Errors loading file $f_name:\\n\";"
      "IRC::echo\"$@\\n\";"
      "return 1;"
    "}"
    "return 0;"
  "}"
  "$SIG{__WARN__}=sub{IRC::echo\"$_[0]\n\";};"
};

Perl::Interp::Interp(WriteFunc stdOut, WriteFunc stdErr)
    : Perl::Std(stdOut, stdErr)
{
    char *embedded[] = {"", "-e", "0"};

    my_perl = perl_alloc();

    /* make sure proper cleaning occurs when interpreter is deleted */
    PL_perl_destruct_level = 1;

    perl_construct(my_perl);
    perl_parse(my_perl, xs_init, 3, embedded, 0);
    perl_run(my_perl);

}

Perl::Interp::~Interp()
{
    perl_destruct(my_perl);
    perl_free(my_perl);
}


#endif