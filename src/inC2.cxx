#include "inC2.h"

#include <algorithm>
#include <cstring>
#include <mpi.h>

InC2::InC2(std::string mpi_args)
{
   // Because we use MPI, we need to have an MPI_Init() ahead of time
   // We want to handle it if the user hasn't, but if they have we shouldn't
   // do it a second time
   MPI_Initialized(&(this->mpi_initialized));
   if(!(this->mpi_initialized))
   {
      if(!mpi_args.empty())
      {
         int n = std::count(mpi_args.begin(), mpi_args.end(), ' ');
         n++;
         char* mpi_args_c = (char*) malloc((1+mpi_args.size()) * sizeof(char)); // Add one for terminal \0
         strcpy(mpi_args_c, mpi_args.c_str());
         char** argv = (char**) malloc((n+1) * sizeof(char*));

         argv[0] = strtok(mpi_args_c, " "); // This mangles cmd_c and uses already allocated memory, so don't need to free
         for(int i = 1; i < n; i++) {
            argv[i] = strtok(NULL, " ");
         }
         argv[n] = NULL; // argv standard requires a NULL string terminator after the actual list of args

         MPI_Init(&n, &argv);

      }
      else
      {
         MPI_Init(NULL, NULL);
      }
   }
}

InC2::~InC2()
{
   // If we did an MPI_Init in the constructor, we should MPI_Finalize as well
   int finalized = 0;
   MPI_Finalized(&finalized);
   if(!(this->mpi_initialized) && !finalized)
   {
      MPI_Finalize();
   }
}

// Create a set of child processes. We pass in the command and the number of processes
// The command is executed as if it were on the command line. For example, if I run
// a job "hoMusic proj.in" on the command line, but I want it to have 20 procs, I could run
// ChildProg("hoMusic proj.in", 20)
InC2Comm* 
InC2::spawnChild(std::string command, int procs)
{
   // MPI is a C lib, so we need to convert our C++ string
   size_t n = std::count(command.begin(), command.end(), ' ');
   char* cmd_c = (char*) malloc((1+command.size()) * sizeof(char)); // Add one for terminal \0
   strcpy(cmd_c, command.c_str());
   char** argv = (char**) malloc((1+n) * sizeof(char*));

   char* exe = strtok(cmd_c, " "); // This mangles cmd_c and uses already allocated memory, so don't need to free
   for(int i = 0; i < n; i++) {
      argv[i] = strtok(NULL, " ");
   }
   argv[n] = NULL; // MPI's list of args is null terminated

   MPI_Comm child_comm;
   MPI_Comm_spawn(cmd_c, argv, procs, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &child_comm, MPI_ERRCODES_IGNORE);

   free(argv);
   free(cmd_c);

   return new InC2Comm(child_comm);
}

// This communicator will only speak with the job's parent.
// If you have an InC2 runner script, then after you use spawnChild() you'll want the
// child apps to each use getParent() to communicate with the runner script.
InC2Comm*
InC2::getParent()
{
   MPI_Comm parent_comm;
   MPI_Comm_get_parent(&parent_comm);

   return new InC2Comm(parent_comm);
}
