#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char uint8;

int width  = 0;
int height = 0;
uint8 *buff = 0;
uint8 *tmp_buff = 0;
const char *out_name = 0;
int first_open_x = 0;
int first_open_y = 0;

struct subimg
{
  int x;
  int y;
  int w;
  int h;
  float cx;
  float cy;
  char name[512];
};

subimg subimgs[512];
int n_subimgs;

int parseArgs(int argc, char **argv)
{
  int i = 1;
  while(i < argc)
  {
         if(strcmp(argv[i],"-w") == 0) { i++; width    = atoi(argv[i]); i++; }
    else if(strcmp(argv[i],"-h") == 0) { i++; height   = atoi(argv[i]); i++; }
    else if(strcmp(argv[i],"-o") == 0) { i++; out_name = argv[i];       i++; }
    else return i;
  }
  return i;
}

void parseName(const char *file_name, char *buff)
{
  int i = 0;
  int start_i = 0;
  int end_i = 0;

  //find end
  while(file_name[end_i] != '\0') end_i++;
  while(end_i >= 0 && file_name[end_i] != '.') end_i--;

  //find start
  start_i = end_i;
  while(start_i >= 0 && file_name[start_i] != '/') start_i--;
  start_i++;

  //shrink end (if float center specified)
  i = end_i;
  i--;
  bool valid = true;;

  //back up through float
  if(valid)
  {
    valid = false;
    while((file_name[i] >= '0' && file_name[i] <= '9')) { i--; valid = true; }
    if(valid && file_name[i] == '.') i--;
    else valid = false;
    if(valid)
    {
      valid = false;
      while((file_name[i] >= '0' && file_name[i] <= '9')) { i--; valid = true; }
    }
  }

  if(valid)
  {
    if(file_name[i] == 'x') i--;
    else valid = false;
  }

  if(valid)
  {
    valid = false;
    while((file_name[i] >= '0' && file_name[i] <= '9')) { i--; valid = true; }
    if(valid && file_name[i] == '.') i--;
    else valid = false;
    if(valid)
    {
      valid = false;
      while((file_name[i] >= '0' && file_name[i] <= '9')) { i--; valid = true; }
    }
  }

  if(valid)
  {
    if(file_name[i] == 'c') i--;
    else valid = false;
  }

  if(valid) end_i = i+1;

  int j = 0;
  while(start_i < end_i) { buff[j] = file_name[start_i]; start_i++; j++; }
  buff[j] = '\0';
}

//sample filename:
//myfilec0.2x0.5.12x34pi
// ^    ^ ^ ^ ^ ^ ^^^ ^
//0: name
//1: c
//2: float
//3: x
//4: float
//5: .
//6: int
//7: x
//8: int
//9: pi
int parseSize(const char *file_name, int *w, int *h, float *cx, float *cy)
{
  int len = strlen(file_name);
  char valbuff[256];
  int i = len-1;
  int j;
  int mark_i;
  while(i > 0 && file_name[i] != '.') i--;
  mark_i = i;

  //width
  j = 0;
  i++;
  while(i < len && file_name[i] != 'x') valbuff[j++] = file_name[i++];
  valbuff[j] = '\0';
  *w = atoi(valbuff);

  //height
  j = 0;
  i++;
  while(i < len && file_name[i] != 'p') valbuff[j++] = file_name[i++];
  valbuff[j] = '\0';
  *h = atoi(valbuff);

  //cx/cy
  i = mark_i;
  i--;
  int valid = true;
  int n_dots;
  while(i > 0 && ((file_name[i] >= '0' && file_name[i] <= '9') || file_name[i] == '.')) i--; //decrement while valid float val
  if(i != mark_i-1 && file_name[i] == 'x') //from 'x'
  {
    mark_i = i;
    j = 0;
    i++;
    n_dots = 0;
    while(i < len && n_dots < 2) //to two dots
    {
      if(file_name[i] == '.') n_dots++;
      if(n_dots < 2) valbuff[j++] = file_name[i++];
    }
    valbuff[j] = '\0';
    *cy = atof(valbuff);

    i = mark_i;
    j = 0;
    i--;
    while(i > 0 && ((file_name[i] >= '0' && file_name[i] <= '9') || file_name[i] == '.')) i--; //decrement while valid float val
    if(i != mark_i-1 && file_name[i] == 'c') //from c
    {
      i++;
      while(i < len && file_name[i] != 'x') valbuff[j++] = file_name[i++]; //to x
      valbuff[j] = '\0';
      *cx = atof(valbuff);
    }
    else valid = false;
  }
  else valid = false;

  if(!valid)
  {
    *cx = 0.5;
    *cy = 0.5;
  }

  return 0;
}

void readFile(const char *file_name, int w, int h)
{
  FILE *fp;
  fp = fopen(file_name,"r");
  fread(tmp_buff,sizeof(uint8),w*h*4,fp);
  fclose(fp);
}

int collides(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
  int mdx = ax-(bx+bw); if(mdx     > 0) return false;
  int mdy = ay-(by+bh); if(mdy     > 0) return false;
  int mdw = aw+bw;      if(mdx+mdw < 0) return false;
  int mdh = ah+bh;      if(mdy+mdh < 0) return false;
  return true;
}

int fits(int x, int y, int w, int h)
{
  if(x < 0)        return false;
  if(y < 0)        return false;
  if(x+w > width)  return false;
  if(y+h > height) return false;
  return true;
}

int findPlacement(subimg *img)
{
  img->x = first_open_x;
  img->y = first_open_y;
  int coll = true;
  int found_first = false;
  while(coll && img->y < height)
  {
    coll = false;
    for(int i = 0; !coll && i < n_subimgs; i++)
      if(collides(img->x,img->y,img->w,img->h,subimgs[i].x,subimgs[i].y,subimgs[i].w,subimgs[i].h)) coll = true;
    if(coll) img->x++;
    if(!coll && !found_first)
    {
      first_open_x = img->x;
      first_open_y = img->y;
      found_first = true;
    }
     if(!coll && !fits(img->x,img->y,img->w,img->h))
    {
      coll = true;
      img->x = 0;
      img->y++;
    }
  }

  return !coll;
}

void placeImg(subimg img)
{
  int src_i;
  int dst_i;
  for(int y = 0; y < img.h; y++)
  {
    for(int x = 0; x < img.w; x++)
    {
      src_i =  (       y *img.w*4)  + (      x)*4;
      dst_i = (((img.y+y)*width*4)) + (img.x+x)*4;
      buff[dst_i+0] = tmp_buff[src_i+0];
      buff[dst_i+1] = tmp_buff[src_i+1];
      buff[dst_i+2] = tmp_buff[src_i+2];
      buff[dst_i+3] = tmp_buff[src_i+3];
    }
  }
  subimgs[n_subimgs++] = img;
}

void appendImg(const char *file_name)
{
  subimg img;
  parseName(file_name, img.name);
  parseSize(file_name, &img.w, &img.h, &img.cx, &img.cy);
  readFile(file_name, img.w, img.h);
  if(findPlacement(&img)) placeImg(img);
  else printf("Could not place %s\n",img.name);
}

void printImg()
{
  char out_file_name[1024];
  sprintf(out_file_name,"%s.%dx%dpi",out_name,width,height);

  FILE *fp;
  fp = fopen(out_file_name,"w");
  fwrite(buff, sizeof(uint8), width*height*4, fp);
  fclose(fp);
}

void printMeta()
{
  char out_file_name[1024];
  sprintf(out_file_name,"%s.pi_meta",out_name);

  FILE *fp;
  fp = fopen(out_file_name,"w");
  fprintf(fp,"%d\n",width);
  fprintf(fp,"%d\n",height);
  fprintf(fp,"%d\n",n_subimgs);
  fprintf(fp,"\n");
  for(int i = 0; i < n_subimgs; i++)
  {
    fprintf(fp,"%s\n",subimgs[i].name);
    fprintf(fp,"%d\n",subimgs[i].x);
    fprintf(fp,"%d\n",subimgs[i].y);
    fprintf(fp,"%d\n",subimgs[i].w);
    fprintf(fp,"%d\n",subimgs[i].h);
    fprintf(fp,"%f\n",subimgs[i].cx);
    fprintf(fp,"%f\n",subimgs[i].cy);
    fprintf(fp,"\n");
  }
  fclose(fp);
}

int main(int argc, char **argv)
{
  int i = 0;
  i = parseArgs(argc, argv);
  if(!width)  width  = 2048;
  if(!height) height = 2048;
  if(!out_name) out_name = "out";

  buff     = (uint8 *)malloc(width*height*4*sizeof(uint8));
  tmp_buff = (uint8 *)malloc(width*height*4*sizeof(uint8));

  for(; i < argc; i++) appendImg(argv[i]);
  printImg();
  printMeta();

  free(buff);
  free(tmp_buff);

  return 0;
}

