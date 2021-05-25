#include "stats.h"


#include "stats.h"

void stats::get_Sum_Helper(PNG &im,int width,int height){

  vector<vector<long>> vec(width, vector<long>(height, 0));
  RGBAPixel* pixel = im.getPixel(0, 0);
  sumRed[0][0] = pixel->r;
  sumGreen[0][0] = pixel->g;
  sumBlue[0][0] = pixel->b;
  sumsqRed[0][0] = (pixel->r) * (pixel->r);
  sumsqGreen[0][0] = (pixel->g) * (pixel->g);
  sumsqBlue[0][0] = (pixel->b) * (pixel->b);

  for (int i = 1; i < width; i++) {
    RGBAPixel* pixel = im.getPixel(i, 0);
    sumRed[i][0] = sumRed[i-1][0] + pixel->r;
    sumGreen[i][0] = sumGreen[i-1][0] + pixel->g;
    sumBlue[i][0] = sumBlue[i-1][0] + pixel->b;
    sumsqRed[i][0] = sumsqRed[i-1][0] + ((pixel->r) * (pixel->r));
    sumsqGreen[i][0] = sumsqGreen[i-1][0] + ((pixel->g) * (pixel->g));
    sumsqBlue[i][0] = sumsqBlue[i-1][0] + ((pixel->b) * (pixel->b));
  }

  for (int j = 1; j < height; j++) {
    RGBAPixel* pixel = im.getPixel(0, j);
    sumRed[0][j] = sumRed[0][j-1] + pixel->r;
    sumGreen[0][j] = sumGreen[0][j-1] + pixel->g;
    sumBlue[0][j] = sumBlue[0][j-1] + pixel->b;
    sumsqRed[0][j] = sumsqRed[0][j-1] + ((pixel->r) * (pixel->r));
    sumsqGreen[0][j] = sumsqGreen[0][j-1] + ((pixel->g) * (pixel->g));
    sumsqBlue[0][j] = sumsqBlue[0][j-1] + ((pixel->b) * (pixel->b));
  }

   
}

stats::stats(PNG & im){
  int width = im.width();
  int height = im.height();
  sumRed.resize(width,vector<long>(height));
  sumGreen.resize(width,vector<long>(height));
  sumBlue.resize(width,vector<long>(height));
  sumsqRed.resize(width,vector<long>(height));
  sumsqGreen.resize(width,vector<long>(height));
  sumsqBlue.resize(width,vector<long>(height));
  get_Sum_Helper(im,width,height);
  for (int i = 1; i < width; i++) {
    for (int j = 1; j <  height; j++) {
      RGBAPixel* pixel = im.getPixel(i, j);
      sumRed[i][j] = sumRed[i-1][j] + sumRed[i][j-1]  - sumRed[i-1][j-1] + pixel->r;
      sumGreen[i][j] = sumGreen[i-1][j] + sumGreen[i][j-1] - sumGreen[i-1][j-1] + pixel->g;
      sumBlue[i][j] = sumBlue[i-1][j] + sumBlue[i][j-1] - sumBlue[i-1][j-1] + pixel->b;
      sumsqRed[i][j] = sumsqRed[i-1][j] + sumsqRed[i][j-1] - sumsqRed[i-1][j-1] + pixel->r*pixel->r;
      sumsqGreen[i][j] =sumsqGreen[i-1][j] + sumsqGreen[i][j-1] - sumsqGreen[i-1][j-1] + pixel->g*pixel->g;
      sumsqBlue[i][j] = sumsqBlue[i-1][j] + sumsqBlue[i][j-1] - sumsqBlue[i-1][j-1] + pixel->b*pixel->b;
    }
  }
}

long stats::handle_get_sum(vector< vector< long >> &sumVect,pair<int,int> ul,int w,int h){
    if(ul.first == 0 && ul.second ==0)
    return sumVect[ul.first + w - 1][ul.second + h -1];
  else if(ul.first == 0 && ul.second != 0)
    return sumVect[ul.first + w - 1][ul.second + h -1] - sumVect[ul.first + w - 1][ul.second -1];
  else if(ul.first != 0 && ul.second == 0)
    return sumVect[ul.first + w - 1][ul.second + h -1] - sumVect[ul.first - 1][ul.second + h -1];
  else return sumVect[ul.first + w - 1][ul.second + h -1] - sumVect[ul.first + w - 1][ul.second -1] -
              sumVect[ul.first - 1][ul.second + h - 1] + sumVect[ul.first -1][ul.second -1];
}

long stats::getSum(char channel, pair<int,int> ul, int w, int h){
  if(channel == 'r'){
    return handle_get_sum(sumRed,ul,w, h);
  }else if (channel == 'g')
    return handle_get_sum(sumGreen,ul, w, h);
    else  return handle_get_sum(sumBlue,ul, w, h);;
}

long stats::getSumSq(char channel, pair<int,int> ul, int w, int h){
  if(channel == 'r'){
    return handle_get_sum(sumsqRed,ul,w,h);
  }else if (channel == 'g')
    return handle_get_sum(sumsqGreen,ul, w, h);
    else  return handle_get_sum(sumsqBlue,ul, w, h);;
}

double stats::compute_var(char channel,pair<int,int> ul, int w, int h){
  return (double)getSumSq(channel,ul,w,h) - ((double)getSum(channel,ul,w,h)*(double)getSum(channel,ul,w,h))/((double)w*(double)h);
}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
// see written specification for a description of this function.
double stats::getVar(pair<int,int> ul, int w, int h){
  if(w == 0 || h==0) return 0;
  return compute_var('r',ul,w,h) + compute_var('g',ul,w,h) + compute_var('b',ul,w,h);
}
		
RGBAPixel stats::getAvg(pair<int,int> ul, int w, int h){
    if(w == 0 || h==0) return RGBAPixel(0,0,0,1);

  double redValue = getSum('r',ul,w,h);
  double greenValue = getSum('g',ul,w,h);
  double blueValue = getSum('b',ul,w,h);
  double area = w * h;
  return RGBAPixel(redValue/area,greenValue/area,blueValue/area,1);
}
