/* SYNC: rg/pkg/util/jftw.c <-> project/tools/util/jftw.c */

/* File tree walker functions.
   Copyright (C) 1996-2001, 2002, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if __GNUC__ && !HAVE_ALLOCA_H
# define alloca __builtin_alloca
#else
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
 #  pragma alloca
#  else
char *alloca ();
#  endif
# endif
#endif

#include <dirent.h>

#include <stdio.h>
#include <errno.h>
#include "jftw.h"
#include <os_includes.h>
#include <limits.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if HAVE_SYS_PARAM_H || defined _LIBC
# include <sys/param.h>
#endif
#ifdef _LIBC
# include <include/sys/stat.h>
#else
# include <sys/stat.h>
#endif

#if ! _LIBC && ! defined HAVE_MEMPCPY && ! defined mempcpy
/* Be CAREFUL that there are no side effects in N.  */
# define mempcpy(D, S, N) ((void *) ((char *) memcpy (D, S, N) + (N)))
#endif

/* #define NDEBUG 1 */
#include <assert.h>

#ifndef _LIBC
# undef __chdir
# define __chdir chdir
# undef __closedir
# define __closedir closedir
# undef __fchdir
# define __fchdir fchdir
# undef __mempcpy
# define __mempcpy mempcpy
# undef __opendir
# define __opendir opendir
# undef __readdir64
# define __readdir64 readdir
# undef __tdestroy
# define __tdestroy tdestroy
# undef __tfind
# define __tfind tfind
# undef __tsearch
# define __tsearch tsearch
# undef internal_function
# define internal_function /* empty */
# undef dirent64
# define dirent64 dirent
# undef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef __set_errno
# define __set_errno(Val) errno = (Val)
#endif

/* Support for the LFS API version.  */
#ifndef FTW_NAME
# define FTW_NAME jftw
# define NFTW_NAME jnftw
# define INO_T ino_t
# define STAT stat
# ifdef _LIBC
#  define LXSTAT __lxstat
#  define XSTAT __xstat
# else
#  define LXSTAT(V,f,sb) lstat (f,sb)
#  define XSTAT(V,f,sb) stat (f,sb)
# endif
# define FTW_FUNC_T __ftw_func_t
# define NFTW_FUNC_T __nftw_func_t
#endif

/* We define PATH_MAX if the system does not provide a definition.
   This does not artificially limit any operation.  PATH_MAX is simply
   used as a guesstimate for the expected maximal path length.
   Buffers will be enlarged if necessary.  */
#ifndef PATH_MAX
# define PATH_MAX 1024
#endif

#define NAMLEN(d) strlen(d->d_name)

struct dir_data
{
  DIR *stream;
  char *content;
};

struct known_object
{
  dev_t dev;
  INO_T ino;
};

struct ftw_data
{
  /* Array with pointers to open directory streams.  */
  struct dir_data **dirstreams;
  size_t actdir;
  size_t maxdir;

  /* Buffer containing name of currently processed object.  */
  char *dirbuf;
  size_t dirbufsize;

  /* Passed as fourth argument to `nftw' callback.  The `base' member
     tracks the content of the `dirbuf'.  */
  struct FTW ftw;

  /* Flags passed to `nftw' function.  0 for `ftw'.  */
  int flags;

  /* Conversion array for flag values.  It is the identity mapping for
     `nftw' calls, otherwise it maps the values to those known by
     `ftw'.  */
  const int *cvt_arr;

  /* Callback function.  We always use the `nftw' form.  */
  NFTW_FUNC_T func;

  /* Device of starting point.  Needed for FTW_MOUNT.  */
  dev_t dev;

  /* Data structure for keeping fingerprints of already processed
     object.  This is needed when not using FTW_PHYS.  */
  void *known_objects;
};


/* Internally we use the FTW_* constants used for `nftw'.  When invoked
   as `ftw', map each flag to the subset of values used by `ftw'.  */
static const int nftw_arr[] =
{
  FTW_F, FTW_D, FTW_DNR, FTW_NS, FTW_SL, FTW_DP, FTW_SLN
};

static const int ftw_arr[] =
{
  FTW_F, FTW_D, FTW_DNR, FTW_NS, FTW_F, FTW_D, FTW_NS
};


/* Forward declarations of local functions.  */
static int ftw_dir (struct ftw_data *data, struct STAT *st) internal_function;


static inline int
add_object (struct ftw_data *data, struct STAT *st)
{
    /* Hopefully, this means that entry has yet been entered and will be
     * proccessed like all enries.
     */
    return 0;
}


static inline int
find_object (struct ftw_data *data, struct STAT *st)
{
    return 0;
}


static inline int
open_dir_stream (struct ftw_data *data, struct dir_data *dirp)
{
  int result = 0;

  if (data->dirstreams[data->actdir] != NULL)
    {
      /* Oh, oh.  We must close this stream.  Get all remaining
	 entries and store them as a list in the `content' member of
	 the `struct dir_data' variable.  */
      size_t bufsize = 1024;
      char *buf = malloc (bufsize);

      if (buf == NULL)
	result = -1;
      else
	{
	  DIR *st = data->dirstreams[data->actdir]->stream;
	  struct dirent64 *d;
	  size_t actsize = 0;

	  while ((d = __readdir64 (st)) != NULL)
	    {
	      size_t this_len = NAMLEN (d);
	      if (actsize + this_len + 2 >= bufsize)
		{
		  char *newp;
		  bufsize += MAX (1024, 2 * this_len);
		  newp = (char *) realloc (buf, bufsize);
		  if (newp == NULL)
		    {
		      /* No more memory.  */
		      int save_err = errno;
		      free (buf);
		      __set_errno (save_err);
		      result = -1;
		      break;
		    }
		  buf = newp;
		}

	      *((char *) __mempcpy (buf + actsize, d->d_name, this_len))
		= '\0';
	      actsize += this_len + 1;
	    }

	  /* Terminate the list with an additional NUL byte.  */
	  buf[actsize++] = '\0';

	  /* Shrink the buffer to what we actually need.  */
	  data->dirstreams[data->actdir]->content = realloc (buf, actsize);
	  if (data->dirstreams[data->actdir]->content == NULL)
	    {
	      int save_err = errno;
	      free (buf);
	      __set_errno (save_err);
	      result = -1;
	    }
	  else
	    {
	      __closedir (st);
	      data->dirstreams[data->actdir]->stream = NULL;
	      data->dirstreams[data->actdir] = NULL;
	    }
	}
    }

  /* Open the new stream.  */
  if (result == 0)
    {
      const char *name = ((data->flags & FTW_CHDIR)
			  ? data->dirbuf + data->ftw.base: data->dirbuf);
      assert (data->dirstreams[data->actdir] == NULL);

      dirp->stream = __opendir (name);
      if (dirp->stream == NULL)
	result = -1;
      else
	{
	  dirp->content = NULL;
	  data->dirstreams[data->actdir] = dirp;

	  if (++data->actdir == data->maxdir)
	    data->actdir = 0;
	}
    }

  return result;
}


static inline int
process_entry (struct ftw_data *data, struct dir_data *dir, const char *name,
	       size_t namlen)
{
  struct STAT st;
  int result = 0;
  int flag = 0;
  size_t new_buflen;

  if (name[0] == '.' && (name[1] == '\0'
			 || (name[1] == '.' && name[2] == '\0')))
    /* Don't process the "." and ".." entries.  */
    return 0;

  new_buflen = data->ftw.base + namlen + 2;
  if (data->dirbufsize < new_buflen)
    {
      /* Enlarge the buffer.  */
      char *newp;

      data->dirbufsize = 2 * new_buflen;
      newp = (char *) realloc (data->dirbuf, data->dirbufsize);
      if (newp == NULL)
	return -1;
      data->dirbuf = newp;
    }

  *((char *) __mempcpy (data->dirbuf + data->ftw.base, name, namlen)) = '\0';

  if ((data->flags & FTW_CHDIR) == 0)
    name = data->dirbuf;

  if (((data->flags & FTW_PHYS)
       ? LXSTAT (_STAT_VER, name, &st)
       : XSTAT (_STAT_VER, name, &st)) < 0)
    {
      if (errno != EACCES && errno != ENOENT)
	result = -1;
      else if (!(data->flags & FTW_PHYS)
	       && LXSTAT (_STAT_VER, name, &st) == 0
	       && S_ISLNK (st.st_mode))
	flag = FTW_SLN;
      else
	flag = FTW_NS;
    }
  else
    {
      if (S_ISDIR (st.st_mode))
	flag = FTW_D;
      else if (S_ISLNK (st.st_mode))
	flag = FTW_SL;
      else
	flag = FTW_F;
    }

  if (result == 0
      && (flag == FTW_NS
	  || !(data->flags & FTW_MOUNT) || st.st_dev == data->dev))
    {
      if (flag == FTW_D)
	{
	  if ((data->flags & FTW_PHYS)
	      || (!find_object (data, &st)
		  /* Remember the object.  */
		  && (result = add_object (data, &st)) == 0))
	    {
	      result = ftw_dir (data, &st);

	      if (result == 0 && (data->flags & FTW_CHDIR))
		{
		  /* Change back to the parent directory.  */
		  int done = 0;
		  if (dir->stream != NULL)
		    if (__fchdir (dirfd (dir->stream)) == 0)
		      done = 1;

		  if (!done)
		    {
		      if (data->ftw.base == 1)
			{
			  if (__chdir ("/") < 0)
			    result = -1;
			}
		      else
			if (__chdir ("..") < 0)
			  result = -1;
		    }
		}
	    }
	}
      else
	result = (*data->func) (data->dirbuf, &st, data->cvt_arr[flag],
				&data->ftw);
    }

  return result;
}


static int
internal_function
ftw_dir (struct ftw_data *data, struct STAT *st)
{
  struct dir_data dir;
  struct dirent64 *d;
  int previous_base = data->ftw.base;
  int result;
  char *startp;

  /* Open the stream for this directory.  This might require that
     another stream has to be closed.  */
  result = open_dir_stream (data, &dir);
  if (result != 0)
    {
      if (errno == EACCES)
	/* We cannot read the directory.  Signal this with a special flag.  */
	result = (*data->func) (data->dirbuf, st, FTW_DNR, &data->ftw);

      return result;
    }

  /* First, report the directory (if not depth-first).  */
  if (!(data->flags & FTW_DEPTH))
    {
      result = (*data->func) (data->dirbuf, st, FTW_D, &data->ftw);
      if (result != 0)
	return result;
    }

  /* If necessary, change to this directory.  */
  if (data->flags & FTW_CHDIR)
    {
      if (__fchdir (dirfd (dir.stream)) < 0)
	{
	  if (errno == ENOSYS)
	    {
	      if (__chdir (data->dirbuf) < 0)
		result = -1;
	    }
	  else
	    result = -1;
	}

      if (result != 0)
	{
	  int save_err = errno;
	  __closedir (dir.stream);
	  __set_errno (save_err);

	  if (data->actdir-- == 0)
	    data->actdir = data->maxdir - 1;
	  data->dirstreams[data->actdir] = NULL;

	  return result;
	}
    }

  /* Next, update the `struct FTW' information.  */
  ++data->ftw.level;
  startp = strchr (data->dirbuf, '\0');
  /* There always must be a directory name.  */
  assert (startp != data->dirbuf);
  if (startp[-1] != '/')
    *startp++ = '/';
  data->ftw.base = startp - data->dirbuf;

  while (dir.stream != NULL && (d = __readdir64 (dir.stream)) != NULL)
    {
      result = process_entry (data, &dir, d->d_name, NAMLEN (d));
      if (result != 0)
	break;
    }

  if (dir.stream != NULL)
    {
      /* The stream is still open.  I.e., we did not need more
	 descriptors.  Simply close the stream now.  */
      int save_err = errno;

      assert (dir.content == NULL);

      __closedir (dir.stream);
      __set_errno (save_err);

      if (data->actdir-- == 0)
	data->actdir = data->maxdir - 1;
      data->dirstreams[data->actdir] = NULL;
    }
  else
    {
      int save_err;
      char *runp = dir.content;

      while (result == 0 && *runp != '\0')
	{
	  char *endp = strchr (runp, '\0');

	  result = process_entry (data, &dir, runp, endp - runp);

	  runp = endp + 1;
	}

      save_err = errno;
      free (dir.content);
      __set_errno (save_err);
    }

  /* Prepare the return, revert the `struct FTW' information.  */
  data->dirbuf[data->ftw.base - 1] = '\0';
  --data->ftw.level;
  data->ftw.base = previous_base;

  /* Finally, if we process depth-first report the directory.  */
  if (result == 0 && (data->flags & FTW_DEPTH))
    result = (*data->func) (data->dirbuf, st, FTW_DP, &data->ftw);

  return result;
}

/* This is implementation of 'get_current_dir_name' that is not dependent on
 * defenition of _GNU_SOURCE.
 */
static char *local_get_current_dir_name(void)
{
    char buf[PATH_MAX];

    if (!getcwd(buf, sizeof(buf)))
	return NULL;

    return strdup(buf);
}

static int
internal_function
ftw_startup (const char *dir, int is_nftw, void *func, int descriptors,
	     int flags)
{
  struct ftw_data data;
  struct STAT st;
  int result = 0;
  int save_err;
  char *cwd = NULL;
  char *cp;

  /* First make sure the parameters are reasonable.  */
  if (dir[0] == '\0')
    {
      __set_errno (ENOENT);
      return -1;
    }

  data.maxdir = descriptors < 1 ? 1 : descriptors;
  data.actdir = 0;
  data.dirstreams = (struct dir_data **) alloca (data.maxdir
						 * sizeof (struct dir_data *));
  memset (data.dirstreams, '\0', data.maxdir * sizeof (struct dir_data *));

  /* PATH_MAX is always defined when we get here.  */
  data.dirbufsize = MAX (2 * strlen (dir), PATH_MAX);
  data.dirbuf = (char *) malloc (data.dirbufsize);
  if (data.dirbuf == NULL)
    return -1;

  cp = strcpy (data.dirbuf, dir) + strlen(dir);
  
  /* Strip trailing slashes.  */
  while (cp > data.dirbuf + 1 && cp[-1] == '/')
    --cp;
  *cp = '\0';

  data.ftw.level = 0;

  /* Find basename.  */
  while (cp > data.dirbuf && cp[-1] != '/')
    --cp;
  data.ftw.base = cp - data.dirbuf;

  data.flags = flags;

  /* This assignment might seem to be strange but it is what we want.
     The trick is that the first three arguments to the `ftw' and
     `nftw' callback functions are equal.  Therefore we can call in
     every case the callback using the format of the `nftw' version
     and get the correct result since the stack layout for a function
     call in C allows this.  */
  data.func = (NFTW_FUNC_T) func;

  /* Since we internally use the complete set of FTW_* values we need
     to reduce the value range before calling a `ftw' callback.  */
  data.cvt_arr = is_nftw ? nftw_arr : ftw_arr;

  /* No object known so far.  */
  data.known_objects = NULL;

  /* Now go to the directory containing the initial file/directory.  */
  if (flags & FTW_CHDIR)
    {
      /* GNU extension ahead.  */
      cwd =  local_get_current_dir_name();
      if (cwd == NULL)
	result = -1;
      else if (data.ftw.base > 0)
	{
	  /* Change to the directory the file is in.  In data.dirbuf
	     we have a writable copy of the file name.  Just NUL
	     terminate it for now and change the directory.  */
	  if (data.ftw.base == 1)
	    /* I.e., the file is in the root directory.  */
	    result = __chdir ("/");
	  else
	    {
	      char ch = data.dirbuf[data.ftw.base - 1];
	      data.dirbuf[data.ftw.base - 1] = '\0';
	      result = __chdir (data.dirbuf);
	      data.dirbuf[data.ftw.base - 1] = ch;
	    }
	}
    }

  /* Get stat info for start directory.  */
  if (result == 0)
    {
      const char *name = ((data.flags & FTW_CHDIR)
			  ? data.dirbuf + data.ftw.base
			  : data.dirbuf);

      if (((flags & FTW_PHYS)
	   ? LXSTAT (_STAT_VER, name, &st)
	   : XSTAT (_STAT_VER, name, &st)) < 0)
	{
	  if (!(flags & FTW_PHYS)
	      && errno == ENOENT
	      && LXSTAT (_STAT_VER, name, &st) == 0
	      && S_ISLNK (st.st_mode))
	    result = (*data.func) (data.dirbuf, &st, data.cvt_arr[FTW_SLN],
				   &data.ftw);
	  else
	    /* No need to call the callback since we cannot say anything
	       about the object.  */
	    result = -1;
	}
      else
	{
	  if (S_ISDIR (st.st_mode))
	    {
	      /* Remember the device of the initial directory in case
		 FTW_MOUNT is given.  */
	      data.dev = st.st_dev;

	      /* We know this directory now.  */
	      if (!(flags & FTW_PHYS))
		result = add_object (&data, &st);

	      if (result == 0)
		result = ftw_dir (&data, &st);
	    }
	  else
	    {
	      int flag = S_ISLNK (st.st_mode) ? FTW_SL : FTW_F;

	      result = (*data.func) (data.dirbuf, &st, data.cvt_arr[flag],
				     &data.ftw);
	    }
	}
    }

  /* Return to the start directory (if necessary).  */
  if (cwd != NULL)
    {
      int save_err = errno;
      __chdir (cwd);
      free (cwd);
      __set_errno (save_err);
    }

  /* Free all memory.  */
  save_err = errno;
  free (data.dirbuf);
  __set_errno (save_err);

  return result;
}



/* Entry points.  */

int
FTW_NAME (path, func, descriptors)
     const char *path;
     FTW_FUNC_T func;
     int descriptors;
{
  return ftw_startup (path, 0, func, descriptors, 0);
}

int
NFTW_NAME (path, func, descriptors, flags)
     const char *path;
     NFTW_FUNC_T func;
     int descriptors;
     int flags;
{
  if (!(flags & FTW_PHYS))
  {
      fprintf(stderr, "You can only use jnftw with the FTW_PHYS flag on.\n");
      exit(1);
  }
  return ftw_startup (path, 1, func, descriptors, flags);
}
