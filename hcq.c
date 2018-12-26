#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name) {
    Student *curr_stu = stu_list;

    // Loop through stu_list to see if student_name in stu_list
    while (curr_stu != NULL){
        // If student_name in stu_list then return pointer
        if (strcmp(curr_stu->name, student_name) == 0){
            return curr_stu;
        }
        // set curr_stu to next student in stu_list

        curr_stu = curr_stu->next_overall;
    }

    // If student_name not found, return NULL
    return NULL;
}

//


/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list. 
 */
Ta *find_ta(Ta *ta_list, char *ta_name) {
    Ta *curr_ta = ta_list;

    // Loop through ta_list to see if ta_name in ta_list
    while (curr_ta != NULL){
        // If ta_name in ta_list then return pointer
        if (strcmp(curr_ta->name, ta_name) == 0){
            return curr_ta;
        }
        // set curr_ta to next ta in ta_list
        curr_ta = curr_ta->next;
    }

    // If ta_name not found, return NULL
    return NULL;
}


/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code) {

    Course *ptr = courses;

    // Loop through courses to see if course_code in courses
    for (int i = 0; i < num_courses; i++){
        if (strcmp(ptr->code, course_code) == 0){
            return ptr;
        }
        ptr++;
    }
    return NULL;

}


/* Add a student to the queue with student_name and a question about course_code.
 * if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the 
 * uniqueness of the name. 
 */
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,
    Course *course_array, int num_courses) {

    // Find student and set to pointer stu_ptr if student exists else set to null
    Student *stu_ptr = find_student(*stu_list_ptr, student_name);

    // If studen't doesn't exist, return 1 and don't create student
    if (stu_ptr != NULL) {
        return 1;
    }

    // Find course in course_array and set to pointer course_ptr if course exists else set to null
    Course *course_ptr = find_course(course_array, num_courses, course_code);

    // If course doesn't exist, return 2 and do not create the student struct
    if (course_ptr == NULL){
        return 2;
    }

    // Initialize malloc for student name and arrival time
    char *name = malloc(sizeof(char) * strlen(student_name) + 1);
    if (name  == NULL) {
        perror("malloc for student name");
        exit(1);
    }
    strcpy(name, student_name);

    // Initialize malloc for arrival time
    time_t *when = malloc(sizeof(time_t));
    if (when  == NULL) {
        perror("malloc for arrival time");
        exit(1);
    }
    *(when) = time(0);

    // Initialize malloc for student itself and add in elements
    Student *student_to_add = malloc(sizeof(Student));
    if (student_to_add  == NULL) {
        perror("malloc for student");
        exit(1);
    }
    student_to_add->name = name;
    student_to_add->arrival_time = when;
    student_to_add->course = course_ptr;
    student_to_add->next_overall = NULL;
    student_to_add->next_course = NULL;

    // Set current_student pointer for iterating over Student structs
    Student *current_student = *stu_list_ptr;

    // If stu_list is empty, add just this ONE student to stu_list
    if (current_student == NULL) {
        *stu_list_ptr = student_to_add;
//        course_ptr->head = student_to_add;
//        course_ptr->tail = student_to_add;
    }

    else {
        if ((*current_student).next_overall != NULL) {
            while (current_student->next_overall != NULL) {
                current_student = current_student->next_overall;
            }
        }

        // Set next_overall of the last student in queue to the new student
        current_student->next_overall = student_to_add;
    }
//    // If the tail is null, it means head is null
//    // set head and tail to student_to_add
//    if (course_ptr->tail == NULL) {
//        course_ptr->head = student_to_add;
//        course_ptr->tail = student_to_add;
//    }
//    // If tail is not null, check which course it is
//    // set that coursestudent_to_add to tail
//        // NEED TO SET HEAD AND TAIL AND NEXT_COURSE for
//    else{
//        course_ptr->tail->next_course = student_to_add;
//        course_ptr->tail = student_to_add;
//    }

    // Add student to course
    if (course_ptr->head == NULL) {
        course_ptr->head = student_to_add;
    }
    if (course_ptr->tail == NULL) {
        course_ptr->tail = student_to_add;
    } else {
        course_ptr->tail->next_course = student_to_add;
        course_ptr->tail = student_to_add;
    }


    return 0;
}


/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */
int give_up_waiting(Student **stu_list_ptr, char *student_name)
{
    Student *student = find_student(*stu_list_ptr, student_name);
    Student *previous_student = NULL;
    Student *previous_student_course = NULL;

    // Check if student exists
    if (student == NULL) {
        return 1;
    }

    // Update stats
    Course *course = student->course;
    course->bailed += 1;
    course->wait_time += difftime(time(NULL), *(student->arrival_time));

    // Clean up the student list
    // Condition 1: the current student is the only one under the course
    if (course->head == course->tail) {
        course->head = NULL;
        course->tail = NULL;
    } else {
        // Find the student before the current one in the course
        previous_student_course = course->head;
        while (previous_student_course->next_course != student) {
            previous_student_course = previous_student_course->next_course;
        }
    }
    // Condition 2: the current student is the only one in the student list
    if ((*stu_list_ptr)->next_overall == NULL) {
        *stu_list_ptr = NULL;
    } else {
        // Find the student before the current one in the list
        previous_student = *stu_list_ptr;
        while (previous_student->next_overall != student) {
            previous_student = previous_student->next_overall;
        }
    }
    // Condition 3: the current student is the last in the student list
    if (student->next_overall == NULL && previous_student != NULL) {
        previous_student->next_overall = NULL;
    }
    // Condition 4: the current student is the last in the course
    if (student->next_course == NULL && previous_student_course != NULL) {
        course->tail = previous_student_course;
        previous_student_course->next_course = NULL;
    }
    // Condition 5: the current student is in the middle of both lists
    if (previous_student != NULL && previous_student_course != NULL) {
        previous_student->next_overall = student->next_overall;
        previous_student_course->next_course = student->next_course;
    }

    // Delete the current student
    free(student->name);
    free(student->arrival_time);
    free(student);

    return 0;
}



//int give_up_waiting(Student **stu_list_ptr, char *student_name) {
//
//    // Find student and set to pointer stu_ptr if student exists else set to null
//    Student *stu_ptr = find_student(*stu_list_ptr, student_name);
//    Student *previous_student = NULL;
//    Student *previous_student_course = NULL;
//
//    if (stu_ptr == NULL){
//        return 1;
//    }
//
//    // RECORD STATS
//    // Update wait_time for students
//    Course *course = stu_ptr->course;
//    course->wait_time += difftime(time(NULL), *(stu_ptr->arrival_time));
//
//    // Add 1 to course's bailed number
//    stu_ptr->course->bailed += 1;


//    // Clean up memory
//    free(stu_ptr->name);
//    free(stu_ptr->arrival_time);
//
//    // CASE 1: If student is start of stu_list
//    if (*stu_list_ptr == stu_ptr){
//        // If student is tail, means student is head, change head and tail pointers to NULL
//        if (stu_ptr->next_course == NULL){
//            stu_ptr->course->head = NULL;
//            stu_ptr->course->tail = NULL;
//        }
//        else{
//            stu_ptr->course->head = stu_ptr->next_course;
//            stu_ptr->next_course = NULL;
//        }
//
//        stu_ptr->course = NULL;
//        // Set stu_list to next student
//        *stu_list_ptr = stu_ptr->next_overall;
//        stu_ptr->next_overall = NULL;
//    }
//
//    // CASE 2: If student is middle of list
//    else{
//        if (stu_ptr->next_overall != NULL){
//            // Set find_overall_before_current to one before stu_ptr
//            Student *find_overall_before_current = *stu_list_ptr;
//            while (find_overall_before_current->next_overall != stu_ptr) {
//                find_overall_before_current = find_overall_before_current->next_overall;
//            }
//
//            //TESTING
////                printf("%s", find_overall_before_current->name);
////                printf("%s", find_overall_before_current->next_overall->name);
//
//            // Set the one overall_before_current's next overall to right one
//            find_overall_before_current->next_overall = stu_ptr->next_overall;
//
//            // Set course_before_current to one course before stu_ptr
//            Student *course_before_current = stu_ptr->course->head;
//            while (course_before_current->next_course != stu_ptr) {
//                course_before_current = course_before_current->next_course;
//            }
//
//            // Set the one before current's next course to NULL
//            course_before_current->next_course = stu_ptr->next_course;
//
//            // If course tail is the last one
//            if (stu_ptr->course->tail == stu_ptr){
//                // If student is also course head, free pointers
//                if (stu_ptr->course->head == stu_ptr){
//                    stu_ptr->course->head = NULL;
//                    stu_ptr->course->tail = NULL;
//                }
//                // If stu_ptr is just tail
//                else {
//                    // Set find_course_before_tail to one before stu_ptr
//                    Student *find_course_before_tail = stu_ptr->course->head;
//                    while (find_course_before_tail->next_course != stu_ptr) {
//                        find_course_before_tail = find_course_before_tail->next_course;
//                    }
//                    // Set the one tail for the course to find_course_before_tail
//                    stu_ptr->course->tail = find_course_before_tail;
//                    find_course_before_tail->next_course = NULL;
//                }
//
//            }
//            // If stu_ptr is just head
//            if (stu_ptr->course->head == stu_ptr){
//                //
//                stu_ptr->course->head = stu_ptr->next_course;
//                stu_ptr->next_course = NULL;
//            }
//
//        }
//
//        // CASE 3: If student is at end of list
//        else{
//            // If course tail is the last one
//            if (stu_ptr->course->tail == stu_ptr){
//                // If student is also course head, free pointers
//                if (stu_ptr->course->head == stu_ptr){
//                    stu_ptr->course->head = NULL;
//                    stu_ptr->course->tail = NULL;
//                }
//                else {
//                    // Set find_overall_before_tail to one before stu_ptr
//                    Student *find_overall_before_tail = *stu_list_ptr;
//                    while (find_overall_before_tail->next_overall != stu_ptr) {
//                        find_overall_before_tail = find_overall_before_tail->next_overall;
//                    }
//                    // Set the one before tail-> next overall to NULL
//                    find_overall_before_tail->next_overall = NULL;
//
//
//                    // Set find_course_before_tail to one before stu_ptr
//                    Student *find_course_before_tail = stu_ptr->course->head;
//                    while (find_course_before_tail->next_course != stu_ptr) {
//                        find_course_before_tail = find_course_before_tail->next_course;
//                    }
//                    // Set the one tail for the course to find_course_before_tail
//                    stu_ptr->course->tail = find_course_before_tail;
//                    find_course_before_tail->next_course = NULL;
//                }
//            }
//
//        }
//    }
//
//    // free student malloc
//    free(stu_ptr);
//}
//
//return 0;
//
//}





/* Create and prepend Ta with ta_name to the head of ta_list. 
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name) {
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL) {
       perror("malloc for TA");
       exit(1);
    }
    new_ta->name = malloc(strlen(ta_name)+1);
    if (new_ta->name  == NULL) {
       perror("malloc for TA name");
       exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student. 
 * Calculate the stats (the times etc.) and then 
 * free the memory for the student. 
 * If the TA has no current student, do nothing.
 */
void release_current_student(Ta *ta) {

    Student *curr_stu = ta->current_student;
    // Check if TA has a current student, if he/she does, release student
    if (curr_stu != NULL){
        Course *current_course = curr_stu->course;

        // Update help_time
        current_course->help_time += difftime(time(0), *(curr_stu->arrival_time));

        // Add 1 to course's helped number
        current_course->helped += 1;

        //Set student's course to NULL
        curr_stu->course = NULL;
    }

}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name) {
    Ta *head = *ta_list_ptr;
    if (head == NULL) {
        return 1;
    } else if (strcmp(head->name, ta_name) == 0) {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL) {
        if (strcmp(head->next->name, ta_name) == 0) {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that 
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}


/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue. 
 * If the queue is empty, then TA ta_name simply finishes with the student 
 * they are currently helping, records appropriate statistics, 
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr) {

    // Check if TA exists
    Ta *ta = find_ta(ta_list, ta_name);
    if (ta == NULL) {
        return 1;
    }

    // Release the current student and free memory
    release_current_student(ta);
    Student *current_student = ta->current_student;

    if (current_student != NULL) {
        free(current_student->name);
        free(current_student->arrival_time);
        free(current_student);
    }

    // Check if anyone in the student list
    if (*stu_list_ptr != NULL) {
        Student *next_student = *stu_list_ptr;
        Course *course = next_student->course;

        // Update stats of wait_time and arrival time
        next_student->course->wait_time += difftime(time(0), *(next_student->arrival_time));
        *(next_student->arrival_time) = time(0);

        // Remove student from the student list
        ta->current_student = next_student;
        *stu_list_ptr = next_student->next_overall;

        // Remove student from the course list
        course->head = next_student->next_course;
        if (course->tail == next_student) {
            course->tail = NULL;
        }
    } else {
        ta->current_student = NULL;
    }

    return 0;
}



/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code. 
 * If no student is waiting for this course, then TA ta_name simply finishes 
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student. 
 */
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code,
        Course *courses, int num_courses) {

    // Check if TA exists
    Ta *ta = find_ta(ta_list, ta_name);
    if (ta == NULL) {
        return 1;
    }

    // Release the current student
    release_current_student(ta);
    Student *current_student = ta->current_student;
    if (current_student != NULL) {
        free(current_student->name);
        free(current_student->arrival_time);
        free(current_student);
    }

    Course *course = find_course(courses, num_courses, course_code);

    // Check if course exists
    if (courses == NULL) {
        return 2;
    }

    // If any student in the course
    if(course->head != NULL) {
        Student *next_student = course->head;
        ta->current_student = next_student;

        // Update stats
        next_student->course->wait_time += difftime(time(0), *(next_student->arrival_time));
        *(next_student->arrival_time) = time(0);

        // Remove student from the course
        course->head = next_student->next_course;
        if (course->tail == next_student) {
            course->tail = NULL;
        }

        // Remove student from student list
        // Check if student is at the head
        if (*stu_list_ptr == next_student) {
            *stu_list_ptr = next_student->next_overall;
        } else {
            // Find the previous student
            Student *previous_student = *stu_list_ptr;
            while (previous_student->next_overall != next_student) {
                previous_student = previous_student->next_overall;
            }

            previous_student->next_overall = next_student->next_overall;
        }
    } else {
        ta->current_student = NULL;
    }
    
    return 0;
}


/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses) {

    // List courses
    Course *courses_ptr = courses;
    for (int i = 0; i < num_courses; i++){
        // Fidn number of students waiting
        Student *stu = courses_ptr[i].head;

        // If no one is in head of that course, return course code and 0
        if (stu == NULL){
            printf("%s: %d in queue\n", courses_ptr[i].code, 0);

        }

        else{
            int num_queue = 0;

            while(stu != NULL){
                num_queue++;
                stu = stu->next_course;
            }

            printf("%s: %d in queue\n", courses_ptr[i].code, num_queue);
        }

        stu = courses_ptr[i].head;
        if (stu != NULL) {
            // Print each student in queue's name
            while (stu != NULL){
                printf("\t%s\n", stu->name);
                stu = stu->next_course;
            }
        }

    }

}


/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements 
 */
void print_currently_serving(Ta *ta_list) {
    // Check if any TA in the list
    if (ta_list == NULL) {
        printf("No TAs are in the help centre.\n");
        return;
    }

    // Loop through TAs
    while(ta_list != NULL) {
        Student *current_student = ta_list->current_student;

        // Check if any student in the list
        if (current_student == NULL) {
            printf("TA: %s has no student\n", ta_list->name);
        } else {
            printf("TA: %s is serving %s from %s\n", ta_list->name,
                    current_student->name, current_student->course->code);
        }

        ta_list = ta_list->next;
    }
}


/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking? 
 */ 
void print_full_queue(Student *stu_list) {
    if (stu_list != NULL) {
        while (stu_list != NULL) {
            printf("Student name %s, arrival_time %ju, course %s\n", stu_list->name,
                    (intmax_t) stu_list->arrival_time, stu_list->course->code);
            stu_list = stu_list->next_overall;
        }
    }
}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {

    // Find course
    Course *found = find_course(courses, num_courses, course_code);

    // Check if course is found, if not return 1
    if (found == NULL){
        return 1;
    }
    else{

        // Check how many students are being helped
        int students_being_helped = 0;
        Ta *current_ta = ta_list;
        while (current_ta != NULL){
            if (current_ta->current_student != NULL){
                students_being_helped++;
            }
            current_ta = current_ta->next;
        }

        // Check how many students are waiting
        int students_waiting = 0;
        Student *waiting_student = found->head;
        while (waiting_student != NULL){
            students_waiting++;
            waiting_student = waiting_student->next_course;
        }

        // You MUST not change the following statements or your code
        //  will fail the testing.

        printf("%s:%s \n", found->code, found->description);
        printf("\t%d: waiting\n", students_waiting);
        printf("\t%d: being helped currently\n", students_being_helped);
        printf("\t%d: already helped\n", found->helped);
        printf("\t%d: gave_up\n", found->bailed);
        printf("\t%f: total time waiting\n", found->wait_time);
        printf("\t%f: total time helping\n", found->help_time);

    }

    return 0;
}


/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename) {

    // Open config file
    FILE *cf = fopen(config_filename, "r");

    if(cf == NULL){
        perror("Error Opening File\n");
        exit(1);
        // fprintf(stderr, "Error Opening File\n");
    }

    // Initialize a buffer for input
    char input_buffer[INPUT_BUFFER_SIZE];

    // Read the first line (number of courses) and construct the course list
    fgets(input_buffer, INPUT_BUFFER_SIZE, cf);
    int num_courses = strtol(input_buffer, NULL, 10);
    Course *course_list = malloc(sizeof(Course) * num_courses);
    *courselist_ptr = course_list;

    // Populate the course list
    for (int i = 0; i < num_courses; i++)
    {
        // Reset buffer and read in course info
        memset(input_buffer, '\0', INPUT_BUFFER_SIZE);
        fgets(input_buffer, INPUT_BUFFER_SIZE, cf);

        // Initialize a course
        Course *current_course = &course_list[i];
        current_course->head = NULL;
        current_course->tail = NULL;
        current_course->helped = 0;
        current_course->bailed = 0;
        current_course->wait_time = 0;
        current_course->help_time = 0;

        // Parse course info and put in the course
        char *token;
        token = strtok(input_buffer, " ");
        strncpy(current_course->code, token, 6); // Split each course into code and description
        current_course->code[6] = '\0';
        int description_length = strlen(&input_buffer[strlen(token) + 1]) + 1;
        current_course->description = malloc(sizeof(char) * description_length);
        strncpy(current_course->description, &input_buffer[strlen(token) + 1], description_length);
        current_course->description[description_length - 2] = '\0';
    }

    fclose(cf);

    return num_courses;
}
