/**************************************************************
 * File:		proj2.c
 * Author:		Šimon Stupinský
 * University: 		Brno University of Technology
 * Faculty: 		Faculty of Information Technology
 * Course:		Operating Systems
 * Date:		23.04.2017
 * Last change:		1.5.2017	
 *		
 * Subscribe:	 	Main module for implementation sychronization problem
 *
 **************************************************************/

/**
 * @file proj2.c
 * @brief Main module for implementation sychronization problem
 */

#include "proj2.h"

void child_work() {
	
	// child starting
	sem_wait(mutex);
		fprintf(file, "%d	: C %d	: started\n",cptr->number++,I_C);
	sem_post(mutex);

	// child enter to center
	sem_wait(mutex);
		// child can be enter without waiting
		if ( cptr->children < 3 * cptr->adults || cptr->adult_count == A ) {
			fprintf(file, "%d	: C %d	: enter\n",cptr->number++,I_C);	
			// increments count of children in center
			cptr->children++;
			sem_post(mutex);
		}
		// child must waiting
		else {
			// increments count of waiting children
			cptr->waiting++;
			fprintf(file, "%d	: C %d	: waiting: %d: %d\n",cptr->number++,I_C,cptr->adults, cptr->children);
			sem_post(mutex);

			// waiting on arrival next adults
			sem_wait(childQueue);

			sem_wait(mutex);
 				fprintf(file, "%d	: C %d	: enter\n",cptr->number++,I_C);	
			sem_post(mutex);
		}

	// simulating activity in center
	if (CWT)
		usleep( random() % CWT  * 1000 );

	// after awakening, children leaving from center
	sem_wait(mutex);
		fprintf(file, "%d	: C %d	: trying to leave\n",cptr->number++,I_C);
		// decrements count of children in center
		cptr->children--;
		fprintf(file, "%d	: C %d	: leave\n",cptr->number++,I_C);
		// decrements sum count of generated process which left
		cptr->sum_count--;
		// if some adult want leaving, check if can leave
		if ( cptr->leaving && cptr->children <= 3 * (cptr->adults-1) ) {
			// decrements count adults who waiting to leave	
			cptr->leaving--;
			// decrements count of adults in center
			cptr->adults--;
			// increments count of adults who left
			cptr->adult_count++;
			// send signal to adult who waiting to leave
			sem_post(adultQueue);
		}
	sem_post(mutex);

	// check if all children and adults left from center
	if (cptr->sum_count == 0)
		// send signal all waiting children and adults, that can finishing
		for ( unsigned i = 0; i < A+C; i++)
			sem_post(commonEnd);

	// waiting for all children and adults	
	sem_wait(commonEnd);

	// children exit
	sem_wait(mutex);
		fprintf(file, "%d	: C %d	: finished\n",cptr->number++,I_C);	
	sem_post(mutex);

	shmdt(cptr);
}

void adult_work() {
	
	//adult starting
	sem_wait(mutex);
		fprintf(file, "%d	: A %d	: started\n",cptr->number++,I_A);
	sem_post(mutex);

	// adult enter to center
	sem_wait(mutex);
		fprintf(file, "%d	: A %d	: enter\n",cptr->number++,I_A);
		// increments count of adults in center
		cptr->adults++;
		// if children waiting, allow them to enter
		if ( cptr->waiting ) {
			// finding count of waiting children
			unsigned n = cptr->waiting > 3 ? 3 : cptr->waiting;
			// send signal to children who waiting to enter
			for ( unsigned i = 0; i < n; i++ )
				sem_post(childQueue);
			// change counts of children in center and waiting children
			cptr->children += n;
			cptr->waiting -= n;
		}
	sem_post(mutex);

	// simulating activity in center
	if (AWT)
		usleep( random() % AWT * 1000);

	// after awakening, adults leaving from center
	sem_wait(mutex); 
		fprintf(file, "%d	: A %d	: trying to leave\n",cptr->number++,I_A);
		// adult can be leaving without waiting
		if ( cptr->children <= 3 * (cptr->adults-1) ) {
			fprintf(file, "%d	: A %d	: leave\n",cptr->number++,I_A);
			// decrements count of adults in center
			cptr->adults--;
			// decrements sum count of generated process which left
			cptr->sum_count--;
			// increments count of adults who left
			cptr->adult_count++;
			sem_post(mutex);
		}
		// adult must waiting, while center left sufficient number of children
		else {
			// increments count of waiting adults to leave
			cptr->leaving++;
			fprintf(file, "%d	: A %d	: waiting: %d: %d\n",cptr->number++,I_A,cptr->adults, cptr->children);
			sem_post(mutex);

			// waiting to signal from child, that can leave
			sem_wait(adultQueue);

			sem_wait(mutex);
				fprintf(file, "%d	: A %d	: leave\n",cptr->number++,I_A);	
				// decrements sum count of generated process which left
				cptr->sum_count--;
			sem_post(mutex);
		}

	// check if all adults left from center
	if ( cptr->adult_count == A && cptr->waiting ) {
		// if children waiting, allow them to enter
		while (cptr->waiting) {
			// send signal to children who waiting to enter
			sem_post(childQueue);
			// increments count of children in center
			cptr->children++;
			// decrements count of waiting children
			cptr->waiting--;
		}
	}	
	
	// check if all children and adults left from center
	if (cptr->sum_count == 0) 
		// send signal all waiting children and adults, that can finishing
		for ( unsigned i = 0; i < A+C; i++ )
			sem_post(commonEnd);
	
	// waiting for all children and adults	
	sem_wait(commonEnd);

	// adult exit
	sem_wait(mutex);
		fprintf(file, "%d	: A %d	: finished\n",cptr->number++,I_A);	
	sem_post(mutex);

	shmdt(cptr);
}

int main(int argc, char *argv[]) {

	errno = 0;

	// loading arguments and their control according to specification
	if (argc == 7) {
		char *ptr;
		// number of adults
		A = strtoul(argv[1], &ptr, RADIX);
		if ( A == 0 || *ptr || argv[1][0] == '-' || errno == ERANGE )
			goto error;

		// number of childrens
		C = strtoul(argv[2], &ptr, RADIX);
		if ( C == 0 || *ptr || argv[2][0] == '-' || errno == ERANGE )
			goto error;

		// adult generate time			
		AGT = strtoul(argv[3], &ptr, RADIX);
		if ( AGT >= 5001 || *ptr || argv[3][0] == '-' || errno == ERANGE )
			goto error;

		// child generate time
		CGT = strtoul(argv[4], &ptr, RADIX);
		if ( CGT >= 5001 || *ptr || argv[4][0] == '-' || errno == ERANGE )
			goto error;
			
		// adult waiting time
		AWT = strtoul(argv[5], &ptr, RADIX);
		if ( AWT >= 5001 || *ptr || argv[5][0] == '-' || errno == ERANGE )
			goto error;
			
		// child waiting time
		CWT = strtoul(argv[6], &ptr, RADIX);
		if ( CWT >=5001 ||  *ptr || argv[6][0] == '-' || errno == ERANGE )
			goto error;
	}
	else {
		error:
		fprintf(stderr, "Wrong list of arguments.\n"
			"**************************************************************************************\n"
			"Using: To enter the correct startup have to lists of six number arguments.\n\n"
			" $ ./proj2 A C AGT CGT AWT CWT\n\n"
			" **-> A: 	number of adults process   -> A > 0\n"
			" **-> C: 	number of children process -> C > 0\n"
			" **-> AGT:	the maximum value of time (in milliseconds) after which a new process for adults is created 		 -> AGT >= 0 && AGT < 5001\n"
			" **-> CGT:	the maximum value of time (in milliseconds) after which a new process for children is created 		 -> CGT >= 0 && CGT < 5001\n"
			" **-> AWT:	the maximum value of time (in milliseconds) for which the adult process simulates activity in the center -> AWT >= 0 && AWT < 5001\n"
			" **-> CWT:	the maximum value of time (in milliseconds) for which the child process simulates activity in the center -> CWT >= 0 && CWT < 5001\n"
			" ** ALL PARAMETERS ARE INTEGER **\n\n"
			"example:  $ ./proj2 1 2 3 4 5 6\n\n"
			"***************************************************************************************\n");
		return 1;
	}


	// opening the output file
	if ( !(file = fopen("proj2.out","w")) ) {
		fprintf(stderr, "Can't create/open the file: \"proj2.out\" \n");
		return 1;
	}
	
	// creating and preparation shared memmory segment
	if ( set_resources() ) {
		perror("Some errors have been caused by function \"set_resources()\" ");
		fclose(file);
		return 2;
	}

	// creating and initializing semaphores
	if ( create_semaphores() ) {
		perror("Some errors have been caused by function \"create_semaphores()\" ");
		free_resources();
		fclose(file);
		return 2;
	}

	// synchronization of output
	setbuf(file, NULL);
 
	// ensuring randomness
	srandom( getpid() );

	cptr->sum_count = A+C;		// ** sum of generated process
	I_A = I_C = 0;			// ** initializing counter for individual process
	pid_t child_pid, adult_pid;	// ** pid variable for creating process

	// creating process ("child maker") which generating needed number of children 
	if ( !(child_pid = fork() ) ) {
		// array for all children pids
		pid_t *child_arr = malloc(sizeof(pid_t)*C);

		// creating children
		for ( unsigned i = 0; i < C; i++) {
			// delays in creator activity
			if (CGT)
				usleep( random() % CGT * 1000);
			if ( !(child_arr[i] = (child_pid = fork()) ) ) {
				// identification of children
				I_C = i+1;
				break;
			}
			// if any of children fail to fork
			else if ( child_pid == -1 ) {
				perror("Children can't be created");
				// kill all created children
				for ( unsigned j = 0; j < i; j++ )
					kill(child_arr[j], SIGTERM);
				// free all allocated resources
				free(child_arr);
				free_resources();
				clear_semaphores();
				// closing file
				fclose(file);
				exit(2);			
			}
		}
		// activity of children
		if ( !(child_pid) ) {
			child_work();
			exit(0);
		}
		// "child maker" waiting for all his created children
		for ( unsigned i = 0; i < C; i++) 
			waitpid(child_arr[i], NULL, 0);	
		// free allocated array
		free(child_arr);
		// "child maker" finish
		exit(0);
	}
	// if "child maker" fails
	else if ( child_pid == -1 ) {
		perror ("Children can't be created");
		// free all allocated resources
		free_resources();
		clear_semaphores();
		// closing file
		fclose(file);
		return 2;
	}
	// creating process ("adult maker") which generating needed number of adults 
	else if ( !(adult_pid = fork() ) ) {
		// array for all adults pids
		pid_t *adult_arr = malloc(sizeof(pid_t)*A);

		// creating adults
		for ( unsigned i = 0; i < A; i++) {
			// delays in creator activity
			if (AGT)
				usleep( random() % AGT * 1000);
			if ( !(adult_arr[i] = (adult_pid = fork()) ) ) {
				// identification of adults
				I_A = i+1;
				break;
			}
			// if any of adults fail to fork
			else if ( adult_pid == -1 ) {
				perror("Adults can't be created");
				// kill all created adults
				for ( unsigned j = 0; j < i; j++)
					kill(adult_arr[j], SIGTERM);
				// free all allocated resources
				free(adult_arr);
				free_resources();
				clear_semaphores();
				// closing file
				fclose(file);
				exit(2);
			}	
		}
		// activity of adults
		if ( !(adult_pid) ) {
			adult_work();
			exit(0);
		}
		// "adult maker" waiting for all his created adults
		for ( unsigned i = 0; i < A; i++ )
			waitpid(adult_arr[i], NULL, 0);
		// free allocated array
		free(adult_arr);
		// "adult maker" finish
		exit(0);
	}
	// if "adult maker" fails
	else if ( adult_pid == -1 ) {
		perror ("Adults can't be created");
		// kill "child maker"
		kill(child_pid, SIGTERM);
		// free all allocated resources
		free_resources();
		clear_semaphores();
		// closing file
		fclose(file);
		return 2;
	}
	// waiting main process on his descendants
	else {
		int status_adult = 0;
		// waiting on "adult maker"
		waitpid(adult_pid, &status_adult, 0);
		// control if all created adults are ok
		if (status_adult) {
			// kill "child maker"
			kill(child_pid, SIGTERM);
			return 2;
		}

		int status_child = 0;
		// waiting on "child maker"
		waitpid(child_pid, &status_child, 0);
		// control if all created children are ok
		if (status_child) {
			// kill "adult maker"
			kill(adult_pid, SIGTERM);
			return 2;
		}
	
		// free all allocated resources
		free_resources();
		clear_semaphores();
	}
	// closing file
	fclose(file);
	return 0;
}
