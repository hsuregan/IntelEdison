/*
 * filter_chirp.c
 * compilation:
 * gcc -o filter_chirp filter_chirp.c -lc -lm -lliquid
 * usage:
 * ./filter_chirp  <input_file_name> <output_file_name>
 * NOTE:
 * If there are not exactly two command line arguments provided if (argc != 3)
 * the program will expect the input file to be named "chirp_data.csv"
 * the output will be saved to a file named "output_data.csv"
 * DETIALS
 * reads in an input signal
 * outputs another signal
 * Perform the filtering as per the assignment guidelines
 */

/* for DSP libraries */
#include <liquid/liquid.h>
/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>

/*
 * Maximum number of samples the buffer can hold 
 * Maximum number of characters for the file names
 */
#define BUFF_SIZE 1024

/*
 * input arguments
 * const char *s = name of array to be printed
 * float complex arr = array to be printed
 * int size = size of array to be printed
 * 
 * eg: 
 * ...
 * float complex a[2];
 * a[0] = 1 + _Complex_I*2;
 * a[1] = 3 + _Complex_I*4;
 * print_arr("a", a, 2);
 * ...
 *
 * -------------------------
 * output:
 * 		a[0] = 1 + j 2
 * 		a[1] = 3 + j 4
 */
void print_arr(const char *s, float complex *arr, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		printf("%s[%3u] = %8.4f + j%8.4f\n", 
				s,
				i,
				crealf(arr[i]),
				cimagf(arr[i])
		      );
	}
}

int main(int argc, char **argv)
{
	/* structures for DSP */
	float input_x[BUFF_SIZE], input_y[BUFF_SIZE];
	float complex *x, *y_orig, *y_filt;
	unsigned int i; 	/* current sample index */
	unsigned int order;	/* filter order */
	unsigned int n; 	/* number of samples in the file */
	float Fc;		/* actual cutoff frequency */
	float Fs;		/* sampling frequency */
	float fc;		/* normalized cutoff frequency Fc/Fs */
	float f0;		/* center frequency for bandpass. Unused. */
	float Ap;		/* pass-band ripple */
	float As;		/* stop-band attenuation */
	/* filters */
	liquid_iirdes_filtertype f_type;
	liquid_iirdes_bandtype b_high_pass, b_low_pass;
	liquid_iirdes_format f_format;
	iirfilt_cccf filter_high_pass, filter_low_pass;

	/* Variables for reading file line by line */
	char *ifile_name, *ofile_name;
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int rv;

	/*
	 * check if the user entered the correct command line arguments
	 * usage: 
	 * ./filter_chirp <input_file_name> <output_file_name>
	 * or 
	 * ./filter_chirp
	 */
	if (argc != 3) {
		ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ifile_name, 0, BUFF_SIZE);
		snprintf(ifile_name, BUFF_SIZE, "chirp_data.csv");
		ofile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_name, 0, BUFF_SIZE);
		snprintf(ofile_name, BUFF_SIZE, "output_data.csv");
	} else {
		ifile_name = argv[1];
		ofile_name = argv[2];
	}

	/* open the input file */
	printf("Attempting to read from file \'%s\'.\n", ifile_name);
	fp = fopen(ifile_name, "r");
	if (fp == NULL) {
		fprintf(stderr, 
				"Failed to read from file \'%s\'.\n", ifile_name
		       );
		exit(EXIT_FAILURE);
	}

	/* read the file, line by line */
	read = getline(&line, &len, fp); //discard header of file
	i = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		/* parse the data */
		rv = sscanf(line, "%f,%f\n", &input_x[i], &input_y[i]);
		if (rv != 2) {
			fprintf(stderr, "Could not parse line[%3u]: \'%s\'\n", 
					i, 
					line
			       );
			exit(EXIT_FAILURE);
		}
		i++;
	}
	fclose(fp);

	n = i;
	/* 
	 * allocate memory for the complex arrays 
	 * float complex *x, *y_orig, *y_filt; 
	 */
	x 	= (float complex *) malloc(sizeof(float complex) * n);
	y_orig 	= (float complex *) malloc(sizeof(float complex) * n);
	y_filt 	= (float complex *) malloc(sizeof(float complex) * n);

	/* populate the arrays with the data */
	for (i = 0; i < n; i++) {
		x[i] = input_x[i];
		y_orig[i] = input_y[i];
	}
	/* displays the contents of "y_orig" */
	// print_arr("y", y_orig, n);

	/* 
	 * reminder of definitions above
	 * liquid_iirdes_filtertype f_type;
	 * liquid_iirdes_bandtype b_high_pass, b_low_pass;
	 * liquid_iirdes_format f_format;
	 * iirfilt_cccf filter_high_pass, filter_low_pass;
	 */

	/*
	 * ONLY MODIFY THIS SECTION
	 */

	/* starting to build the filters */
	f_type 		= 0;	/* filter type: Butterworth, etc */
	b_high_pass 	= 0;	/* High-pass filter */
	b_low_pass 	= 0;	/* Low-pass filter */
	f_format 	= 0;	/* Second Order Section form */
	Fs 		= 1.0f;	/* Sampling frequency (Hz) */

	Fc    = 1.0f; 	/* Cut off frequency (Hz) */
	f0    = 1.0f; 	/* Ignored for high pass and low pass */
	Ap    = 1.0f; 	/* Pass band ripple (dB), ignored for Butterworth */
	As    = 1.0f; 	/* Stop band ripple (dB), ignored for Butterworth */
	order = 1; 	/* Filter order */
	fc    = Fc/Fs; 	/* Normalized cutoff frequency */

	/*
	 * DO NOT MODIFY ANYTHING PAST THIS COMMENT
	 */

	/* open the output file to write the data */
	printf("Attempting to write to file \'%s\'.\n", ofile_name);
	fp = fopen(ofile_name, "w");
	if (fp == NULL) {
		fprintf(stderr, 
				"Failed to write to file \'%s\'.\n", ofile_name
		       );
		exit(EXIT_FAILURE);
	}

	/* 
	 * print the data to a file 
	 * sample: i
	 * x = x[i], original "time series"
	 * y_orig_r = real component of original signal values
	 * y_orig_i = imaginary component of original signal values
	 * y_filt_r = real component of filtered signal values
	 * y_filt_i = imaginary component of filtered signal values
	 */
	fprintf(fp, "sample,x,y_orig_r,y_filt_i,y_filt_r,y_filt_i\n");
	for (i = 0; i < n; i++) {
		fprintf(fp, "%d,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f\n",
				i,
				crealf(x[i]),
				crealf(y_orig[i]),
				cimagf(y_orig[i]),
				crealf(y_filt[i]),
				cimagf(y_filt[i])
		       );
	}
	fclose(fp);
	return 0;
}
