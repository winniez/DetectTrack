
#pragma once
#ifndef PARTICALITEM_H
#define PARTICALITEM_H

#include "defs.h"
#include "particles.h"

/***************************** Function Prototypes ***************************/
histogram* compute_ref_histos( IplImage* frame, CvRect region);

/******************************** Classes ***********************************/
class ParticalItem
{
private:
	particle* particles, * new_particles;
    histogram* ref_histos;
	CvRect resultrect;
	int num_particles;
public:
	
	ParticalItem(){num_particles=150;ref_histos=NULL;particles=NULL; new_particles=NULL; }
	void Init(IplImage* hsv_frame, CvRect &region);
	CvRect tracking(IplImage* hsv_frame, gsl_rng* rng);
	
	~ParticalItem()
	{
		delete ref_histos; 
		delete []particles; //delete []new_particles;
		ref_histos=NULL;particles=NULL; new_particles=NULL;
		//free(particles);free(new_particles);free(ref_histos);
	}
};

void ParticalItem::Init(IplImage* hsv_frame, CvRect &region)
{
	/* compute reference histograms and distribute particles */
	ref_histos = compute_ref_histos( hsv_frame, region);
	particles = init_distribution(region, ref_histos, num_particles);
	//particles = init_distribution( regions, ref_histos,num_objects, num_particles );
}

CvRect ParticalItem::tracking(IplImage* hsv_frame, gsl_rng* rng)
{
	int w = hsv_frame->width, h = hsv_frame->height;
	float s=0;
	/* perform prediction and measurement for each particle */
	for( int j = 0; j < num_particles; j++ )
	{
		particles[j] = transition( particles[j], w, h, rng );
		s = particles[j].s;
		particles[j].w = likelihood( hsv_frame, cvRound(particles[j].y),
					cvRound( particles[j].x ),
					cvRound( particles[j].width * s ),
					cvRound( particles[j].height * s ),
					particles[j].histo );
	}
	/* normalize weights and resample a set of unweighted particles */
	normalize_weights( particles, num_particles );
	new_particles = resample( particles, num_particles );
	free( particles );
	particles = new_particles;
	/* display all particles if requested */
    qsort( particles, num_particles, sizeof( particle ), &particle_cmp );

	int x0, y0, x1, y1;
	x0 = cvRound( particles[0].x - 0.5 * particles[0].s * particles[0].width );
	y0 = cvRound( particles[0].y - 0.5 * particles[0].s * particles[0].height );
	x1 = cvRound( particles[0].s * particles[0].width );
	y1 = cvRound( particles[0].s * particles[0].height );
	resultrect = cvRect(x0,y0,x1,y1);
	return resultrect;
}


/*
  Computes a reference histogram for each of the object regions defined by
  the user

  @param frame video frame in which to compute histograms; should have been
    converted to hsv using bgr2hsv in observation.h
  @param regions regions of \a frame over which histograms should be computed
  @param n number of regions in \a regions
  @param export if TRUE, object region images are exported

  @return Returns an \a n element array of normalized histograms corresponding
    to regions of \a frame specified in \a regions.
*/
histogram* compute_ref_histos( IplImage* frame, CvRect region)
{
	//histogram** histos = malloc( n * sizeof( histogram* ) ); //TYS_B
	//int n=1;
	histogram* histos = new histogram;
	IplImage* tmp;

    // extract region from frame and compute its histogram 
    cvSetImageROI( frame, region );
	tmp = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 3 );
	cvCopy( frame, tmp, NULL );
	cvResetImageROI( frame );
	histos = calc_histogram( &tmp, 1 );
	normalize_histogram( histos );
	cvReleaseImage( &tmp );

  return histos;
}


#endif