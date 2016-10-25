#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int width = 0;
int height = 0;
char *buff = 0;
char *tmp_buff = 0;
char *out_name = 0;

struct subimg
{
  int x;
  int y;
  int w;
  int h;
  const char *name;
};

subimg subimgs[100];
int n_subimgs;

int parseArgs(int argc, char **argv)
{
  int i = 0;
  while(i < argc)
  {
         if(strcmp(argv[i],"-w")) { i++; width    = atoi(argv[i]); i++; }
    else if(strcmp(argv[i],"-h")) { i++; height   = atoi(argv[i]); i++; }
    else if(strcmp(argv[i],"-o")) { i++; out_name = argv[i];       i++; }
    else return i;
  }
  return i;
}

int parseSize(const char *file_name, int *w, int *h)
{
  int len = strlen(file_name);
  char intbuff[256];
  int i = len-1;
  int j;
  while(i > 0   && file_name[i] != '.') i--;

  j = 0;
  i++;
  while(i < len && file_name[i] != 'x') intbuff[j++] = file_name[i++];
  intbuff[j] = '\0';
  *w = atoi(intbuff);

  j = 0;
  i++;
  while(i < len && file_name[i] != 'p') intbuff[j++] = file_name[i++];
  intbuff[j] = '\0';
  *h = atoi(intbuff);

  return 0;
}

void readFile(const char *file_name, int w, int h)
{
  FILE *fp;
  fp = fopen(file_name,"r");
  fgets(tmp_buff, w*h*4, fp);
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
  img->x = 0;
  img->y = 0;
  int coll = true;
  while(coll && img->y < height)
  {
    coll = false;
    for(int i = 0; i < n_subimgs; i++)
      if(collides(img->x,img->y,img->w,img->h,subimgs[i].x,subimgs[i].y,subimgs[i].w,subimgs[i].h)) coll = true;
    if(coll)
    {
      img->x++;
    }
    else if(!fits(img->x,img->y,img->w,img->h))
    {
      coll = true;
      img->x = 0;
      img->y++;
    }
  }

  return coll;
}

void placeImg(subimg img)
{
  for(int y = 0; y < img.h; y++)
    for(int x = 0; x < img.w; y++)
      buff[((img.y+y)*width) + (img.x+x)] = tmp_buff[(y*img.w) + x];
  subimgs[n_subimgs++] = img;
}

void appendImg(const char *file_name)
{
  subimg img;
  img.name = file_name;
  parseSize(file_name, &img.w, &img.h);
  readFile(file_name, img.w, img.h);
  findPlacement(&img);
  placeImg(img);
}

void printImg()
{
  char out_file_name[1024];
  sprintf(out_file_name,"%s.%dx%dpi",out_name,width,height);

  FILE *fp;
  fp = fopen(out_file_name,"w");
  fwrite(buff, sizeof(char), width*height*4, fp);
  fclose(fp);
}

void printMeta()
{
  char out_file_name[1024];
  sprintf(out_file_name,"%s.%dx%dpi_meta",out_name,width,height);

  FILE *fp;
  fp = fopen(out_file_name,"w");
  fprintf(fp,"%d\n",width);
  fprintf(fp,"%d\n",height);
  fprintf(fp,"\n");
  for(int i = 0; i < n_subimgs; i++)
  {
    fprintf(fp,"%s\n",subimgs[i].name);
    fprintf(fp,"%d\n",subimgs[i].x);
    fprintf(fp,"%d\n",subimgs[i].y);
    fprintf(fp,"%d\n",subimgs[i].w);
    fprintf(fp,"%d\n",subimgs[i].h);
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

  buff     = (char *)malloc(width*height*4*sizeof(char));
  tmp_buff = (char *)malloc(width*height*4*sizeof(char));

  for(; i < argc; i++) appendImg(argv[i]);
  printImg();
  printMeta();

  free(buff);
  free(tmp_buff);

  return 0;
}

