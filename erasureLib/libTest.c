#include <erasure.h>

#ifndef __MARFS_COPYRIGHT_H__
#define __MARFS_COPYRIGHT_H__

/*
Copyright (c) 2015, Los Alamos National Security, LLC
All rights reserved.

Copyright 2015.  Los Alamos National Security, LLC. This software was produced
under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National
Laboratory (LANL), which is operated by Los Alamos National Security, LLC for
the U.S. Department of Energy. The U.S. Government has rights to use, reproduce,
and distribute this software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL
SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
FOR THE USE OF THIS SOFTWARE.  If software is modified to produce derivative
works, such modified software should be clearly marked, so as not to confuse it
with the version available from LANL.
 
Additionally, redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. Neither the name of Los Alamos National Security, LLC, Los Alamos National
Laboratory, LANL, the U.S. Government, nor the names of its contributors may be
used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-----
NOTE:
-----
Although these files reside in a seperate repository, they fall under the MarFS copyright and license.

MarFS is released under the BSD license.

MarFS was reviewed and released by LANL under Los Alamos Computer Code identifier:
LA-CC-15-039.

These erasure utilites make use of the Intel Intelligent Storage Acceleration Library (Intel ISA-L), which can be found at https://github.com/01org/isa-l and is under its own license.

MarFS uses libaws4c for Amazon S3 object communication. The original version
is at https://aws.amazon.com/code/Amazon-S3/2601 and under the LGPL license.
LANL added functionality to the original work. The original work plus
LANL contributions is found at https://github.com/jti-lanl/aws4c.

GNU licenses can be found at http://www.gnu.org/licenses/.
*/

#endif

#include <time.h>

int main( int argc, const char* argv[] ) 
{
   int nread;
   void *buff;
   int toread;
   int start;
   char write;
   int filefd;
   int N;
   int E;

   if ( argc < 2 ) {
      fprintf( stderr, "libTest: no operation (read/write) was specified!\n");
      return -1;
   }

   if ( strncmp( argv[1], "write", strlen(argv[1]) ) == 0 ) {
      if ( argc < 7 ) { 
         fprintf(stderr,"libTest: insufficient arguments for a write, expected %s %s input_file output_path N E start_file\n", argv[0], argv[1] ); 
         return -1;
      }
      write = 1;
   }
   else if ( strncmp( argv[1], "read", strlen(argv[1]) ) == 0 ) {
      if ( argc < 7 ) { 
         fprintf(stderr,"libTest: insufficient arguments for a read, expected %s %s output_file erasure_path N E start_file\n", argv[0], argv[1] ); 
         return -1;
      }
      write = 0;
   }
   else {
      fprintf( stderr, "libTest: argument 1 not recognized, expecting \"read\" or \"write\"\n" );
      return -1;
   }
   
   N = atoi(argv[4]);
   E = atoi(argv[5]);
   start = atoi(argv[6]);
   buff = malloc( sizeof(char) * (N+E) * 64 * 1024 );

   filefd = open( argv[2], O_RDONLY );
   if ( filefd == -1 ) {
      fprintf( stderr, "libTest: failed to open file %s\n", argv[2] );
      return -1;
   }
   
   srand(time(NULL));
   ne_handle handle;

   if ( write ) {
      handle = ne_open( (char *)argv[3], NE_WRONLY, start, N, E );
      if ( handle == NULL ) {
         fprintf( stderr, "libTest: ne_open failed\n   Errno: %d\n   Message: %s\n", errno, strerror(errno) );
         return -1;
      }

      toread = rand() % ( N * 64 * 1024 );

      while ( (nread = read( filefd, buff, toread )) != 0 ) {
         if ( nread != ne_write( handle, buff, nread ) ) {
            fprintf( stderr, "libTest: unexpected # of bytes written by ne_write\n" );
            return -1;
         }

         toread = rand() % ( N * 64 * 1024 );
      }

   }
   else if ( ! write ) {
      ;
   }

   ne_close( handle );

   return EXIT_SUCCESS;
}