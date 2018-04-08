/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 *
 * Copyright: 2012-2018 Boomaga team https://github.com/Boomaga
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include <errno.h>
#include <glob.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "tools.h"
#include "logging.h"

#define PATTERN "*.boo"
#define OPEN_COMMAND "/usr/bin/open"

using namespace std;

/************************************************
 *
 ************************************************/
void usage()
{
    fprintf(stderr, "Usage: agent source_direcory target_direcory application_id\n");
    fprintf(stderr, "  example: agent /var/spool/io.github.Boomaga "
                    "'~/Library/Application Support/io.github.Boomaga' "
                    "io.github.Boomaga\n");
}


/************************************************
 *
 ************************************************/
string expandSrcPath(const string &path)
{
    string res = path;
    if (res[0] == '~')
    {
        const char *home = getenv("HOME");
        if (!home)
        {
            error("can't get HOME environment variable");
            return NULL;
        }

        res.replace(0, 1, home);
    }

    const char *user = getenv("USER");
    if (!user)
    {
        error("can't get USER environment variable");
        return "";
    }

    return res + "/" + user;
}


/************************************************
 *
 ************************************************/
string expandDestPath(const char *path)
{
    string res = path;
    if (res[0] == '~')
    {
        const char *home = getenv("HOME");
        if (!home)
        {
            error("can't get HOME environment variable");
            return NULL;
        }

        res.replace(0, 1, home);
    }

    return res;
}


/************************************************
 *
 ************************************************/
void startApplication(const string &app, const string &file)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        throw format("can't fork: ", strerror(errno));
    }

    if (pid)
    {
        debug("forked, chaild PID is %d", pid);
        // wait for the child to exit
        int status;
        waitpid(pid, &status, 0);
        debug("%s finished with %d exit code.", OPEN_COMMAND, status);
        if (status != 0)
            error("%s finished with %d exit code.", OPEN_COMMAND, status);
        return;
    }

    else
    {
        debug("exec '%s' '%s' '%s' '%s' ",
              OPEN_COMMAND,
              "-b", app.c_str(),
              file.c_str());

        execl(OPEN_COMMAND,
              OPEN_COMMAND,
              "-b", app.c_str(),
              file.c_str(),
              nullptr);
    }
}


/************************************************

 ************************************************/
int main(int argc, char *argv[])
{
    if (argc != 4 ||
        !strlen(argv[1]) ||
        !strlen(argv[2]) ||
        !strlen(argv[3])  )
    {
        usage();
        return 1;
    }

    debug("start '%s' '%s' '%s'", argv[1], argv[2], argv[3]);

    try
    {
        string srcDir  = expandSrcPath(argv[1]);
        string destDir = expandDestPath(argv[2]);
        string app     = argv[3];

        if (srcDir == destDir)
        {
            warn("source_direcory (\"%s\") and target_direcory (\"%s\") must be different.\n", srcDir.c_str(), destDir.c_str());
            return 400;
        }

        debug("srcDir:  '%s'", srcDir.c_str());
        debug("destDir: '%s'", destDir.c_str());

        mkdirs(destDir, 0700);

        do{
            glob_t files;
            int ret = glob((srcDir + "/" + PATTERN).c_str(), GLOB_TILDE, 0, &files);

            if (ret == GLOB_NOMATCH)
            {
                globfree(&files);
                debug("no more files, exit");
                return 0;
            }

            if (ret != 0)
            {
                error(strerror(errno));
                return 500;
            }

            for (size_t i=0; i<files.gl_pathc; ++i)
            {
                debug("foud file '%s'", files.gl_pathv[i]);
                string outFile = destDir + "/" + basename(files.gl_pathv[i]);

                debug("move to '%s'", outFile.c_str());
                if (rename(files.gl_pathv[i], outFile.c_str()) < 0)
                {
                    throw format("can't move file \"%s\" to \"%s\": %s",
                             files.gl_pathv[i],
                             destDir.c_str(),
                             strerror(errno));
                }

                debug("start application '%s' with file '%s'", app.c_str(), outFile.c_str());
                startApplication(app, outFile);
            }

            globfree(&files);
        } while(true); // If new files appear in the directory during this cycle, we process them again.

    }
    catch (string &e)
    {
        fflush(stdout);
        error(e.c_str());
        return 500;
    }

    return 0;
}
