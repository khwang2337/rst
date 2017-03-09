#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 rotate: create an rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 2 arguments (axis, theta) axis should be x y or z
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  char INPUT;
  char line[256];
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) {
    f = stdin;
    INPUT = 1;
  }
  else {
    f = fopen(filename, "r");
    INPUT = 0;
  }
  
  char * buffer;
  char * commandline;
  double x, y, z, x1, y1, z1, t;
  char axis, gate;
  
  color c;
  c.red = 0;
  c.green = MAX_COLOR;
  c.blue = 0;
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);
    
    buffer = line;
    
    if (! strcmp(line, "line")) {
        if (INPUT) {
            printf("Input coordinates: ");
            fgets(line, 255, f);
            buffer = line;
        }
        else {
            fgets(line, 255, f);
            line[strlen(line)-1]='\0';
            buffer = line;
        }
        commandline = strsep(&buffer, "\n");
        sscanf(commandline, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &x1, &y1, &z1);
        add_point(edges, x, y, z);
        add_point(edges, x1, y1, z1);
        //printf("EDGES: \n");
        //print_matrix(edges);
    }
    
    else if (! strcmp(line, "ident")) {
        ident(transform);
        //printf("TRANSFORM: \n");
        //print_matrix(transform);
    }
    
    else if (! strcmp(line, "scale")) {
        if (INPUT) {
            printf("Input xscale yscale zscale: ");
            fgets(line, 255, f);
            buffer = line;
        }
        else {
            fgets(line, 255, f);
            line[strlen(line)-1]='\0';
            buffer = line;
        }
        commandline = strsep(&buffer, "\n");
        sscanf(commandline, "%lf %lf %lf", &x, &y, &z);
        
        struct matrix * scale = make_scale(x, y, z);
        matrix_mult(scale, transform);
        
        //printf("TRANSFORM: \n");
        //print_matrix(transform);
    }
    
    else if (! strcmp(line, "move")) {
        if (INPUT) {
            printf("Input deltax deltay deltaz: ");
            fgets(line, 255, f);
            buffer = line;
        }
        else {
            fgets(line, 255, f);
            line[strlen(line)-1]='\0';
            buffer = line;
        }
        commandline = strsep(&buffer, "\n");
        sscanf(commandline, "%lf %lf %lf", &x, &y, &z);
        
        struct matrix * translate = make_translate(x, y, z);
        matrix_mult(translate, transform);
        
        //printf("TRANSFORM: \n");
        //print_matrix(transform);
    }
    
    else if (! strcmp(line, "rotate")) {
        gate = 1;
        if (INPUT) {
            printf("Input axis theta: ");
            fgets(line, 255, f);
            buffer = line;
        }
        else {
            fgets(line, 255, f);
            line[strlen(line)-1]='\0';
            buffer = line;
        }
        commandline = strsep(&buffer, "\n");
        sscanf(commandline, "%c %lf", &axis, &t);
        
        struct matrix * rotate;
        if (axis == 'x') rotate = make_rotX(t);
        else if (axis == 'y') rotate = make_rotY(t);
        else if (axis == 'z') rotate = make_rotZ(t);
        else {
            printf("Rotate by an invalid axis\n");
            gate = 0;
        }
        
        if (gate) {
            matrix_mult(rotate, transform);
            printf("TRANSFORM: \n");
            print_matrix(transform);
        }
    }
    
    else if (! strcmp(line, "apply")) {
        matrix_mult(transform, edges);
        
        printf("EDGES: \n");
        print_matrix(edges);
    }
    
    else if (! strcmp(line, "display")) {
        draw_lines(edges, s, c);
        display(s);
    }
    
    else if (! strcmp(line, "save")) {
        if (INPUT) {
            printf("Input filename: ");
            fgets(line, 255, f);
            buffer = line;
        }
        else {
            fgets(line, 255, f);
            line[strlen(line)-1]='\0';
            buffer = line;
        }
        draw_lines(edges, s, c);
        commandline = strsep(&buffer, "\n");
        save_extension(s, commandline);
    }
    
    else if (! strcmp(  line, "show")) {
        printf("EDGES: \n");
        print_matrix(edges);
        
        printf("\nTRANSFORM: \n");
        print_matrix(transform);
    }
    printf("\n");

  }
}
  
