#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_STATION 247 // commence à 0 dans dot
#define NB_CHAR 70
#define NB_LIGNE_DOT 256

typedef struct{
	int * tab;
	int debut;
	int nbElement;
} mafile;

mafile* creation_file()
{
	mafile *f=malloc(sizeof(f));
	f->tab=malloc(NB_STATION * sizeof(int));
	f->debut=0;
	f->nbElement=0;
	return f;
}

int estVide(mafile *f)
{
	if (f->nbElement==0) return 1;
	return 0;
}

int estPlein(mafile *f)
{
	if (f->nbElement==NB_STATION) return 1;
	return 0;
}

void enfiler(mafile *f, int sommet)
{
	int position;
	if (!estPlein(f))
	{
		position=(f->debut+f->nbElement); //On ne peut pas dépasser le tableau car on prend au maximum toutes les stations
		f->tab[position]=sommet;
		f->nbElement=f->nbElement+1;
		return;
	}
	exit(1);
}

int defiler(mafile *f)
{
	int val;
	if (!estVide(f))
	{
		val=f->tab[f->debut];
		f->debut=f->debut+1;
		f->nbElement=f->nbElement-1;
		return val;
	}
	exit(1);
}

void generer_image(char dot[],char svg[],char extension[])
{
	char c[50];
	sprintf((char*)c,"neato -T%s %s -o %s.%s",extension,dot,svg,extension); 
	system(c);
}

void affichage(char ** nom_station)
{
	int i;
	int j;
  	for(i=0;i<NB_STATION;++i)
  	{
    	for(j=0;j<70;++j)
		{
			printf("%c",nom_station[i][j]);
		}
      	printf("\n");
  	}
}


void creation_nom_station(char ** nom_station,char fichiertxt[])
{
	int i;
	int j=0;
	char *tmp;
	tmp=malloc(NB_CHAR*sizeof(char));
	FILE *txt = fopen(fichiertxt,"r");
  	while(fgets(tmp,NB_CHAR,txt)!=NULL)
  	{
    	for(i=4;tmp[i]!='\n';++i)
    	{
	 		nom_station[j][i-4]=tmp[i];
		}
		nom_station[j][i-5]='0';		//-5 car \n est quand meme écrit 
    	++j;
  	}
  	
  	fclose(txt);
  	free(tmp);
  	return ;
}

void creation_matrice(int ** mat, char fichierdot[])
{
	FILE *dot= fopen(fichierdot,"r");
	int i;
	char pble[50];
	int a,b;
	for(i=0;i<4;i++)
	{
		fgets(pble,50,dot);
	}
	for(i=0;i<NB_LIGNE_DOT;++i)
	{
		
		fscanf(dot,"%d -- %d",&a,&b);
		mat[a][b]=1;
		mat[b][a]=1;
	}
}

int parcours_largeur(int ** mat,int s) //Si pont trouvé, mat[x][y]=2 ou Si double pont trouvé les arrêtes valent 3
{	
	mafile *f=creation_file();
	int j,i, test_non_connexe=0;
	int *marquer=calloc(NB_STATION, sizeof(int));
	/*if(x>0 && y>0 && x<NB_STATION && y<NB_STATION)
		mat[x][y]=mat[y][x]=0;*/
	enfiler(f,s);
	marquer[s]=1; //on le met en gris
	while(!estVide(f))
	{
		s=defiler(f);
		i=s;
		j=0;
		while(j<NB_STATION)
		{	
			if (mat[i][j]==1 || mat[i][j]==2)
			{
				if  (marquer[j]==0) //Non marqué
				{
					enfiler(f,j);
					marquer[j]=1;
				}
			}
			j++;
		}
	}
	for (i=0; i<NB_STATION; i++)
	{
		if (marquer[i]==0) 
		{
			test_non_connexe=1;
			break;
		}
	}
	/*if (x>0 && y>0 && x<NB_STATION && y<NB_STATION)
	{
		if (test_non_connexe) 
			mat[x][y]=mat[y][x]=2;
		else
			mat[x][y]=mat[y][x]=1;
	}*/
	/*
	if(test_non_connexe)
		printf("Non Connexe\n");
	else
		printf("Connexe\n");*/
	free(marquer);
	free(f->tab);
	free(f);
	return test_non_connexe;
}

void cherche_pont(int **mat,char **nom) 
{
	FILE *fichier=fopen("Liste_pont.txt","w");
	int i;
	int j;
	int k;
	for(i=0;i<NB_STATION;i++)
	{
		for(j=0;j<NB_STATION;j++)
		{
			if(mat[i][j]==1)
			{
				mat[i][j]=mat[j][i]=0;
				if(parcours_largeur(mat,0))
				{
					mat[i][j]=mat[j][i]=2;
					for(k=0;nom[i][k]!='0';k++)
					{
						fprintf(fichier,"%c",nom[i][k]);
					}
					fprintf(fichier," -- ");
					for(k=0;nom[j][k]!='0';k++)
					{
						fprintf(fichier,"%c",nom[j][k]);
					}
					fprintf(fichier,"\n");
				}
				else
				{
					mat[i][j]=mat[j][i]=1;
				}
				
			}
		}
		
	}
	fclose(fichier);
}

void cherche_double_pont(int **mat,char **nom)
{
	FILE *fichier=fopen("Liste_double_pont","w");
	int i;
	int j;
	int k;
	int l;
	for(i=0;i<NB_STATION;i++)
	{
		for(j=0;j<NB_STATION;j++)
		{
			for(k=0;k<NB_STATION;k++)
			{
				if(k!=j && ((mat[i][j]==1 && mat[i][k]==1) || (mat[i][j]==3 && mat[i][k]==1) || (mat[i][j]==1 && mat[i][k]==3)))
				{
					mat[i][j]=mat[j][i]=0;
					mat[i][k]=mat[k][i]=0;
					if(parcours_largeur(mat,0))
					{
						for(l=0;nom[j][l]!='0';l++)
						{
							fprintf(fichier,"%c",nom[j][l]);
						}
						fprintf(fichier," -- ");
						for(l=0;nom[i][l]!='0';l++)
						{
							fprintf(fichier,"%c",nom[i][l]);
						}
						fprintf(fichier," -- ");
						for(l=0;nom[k][l]!='0';l++)
						{
							fprintf(fichier,"%c",nom[k][l]);
						}
						fprintf(fichier,"\n");
					}
					mat[i][j]=mat[j][i]=3;
					mat[i][k]=mat[k][i]=3;
				}

			}


		}


	}

	fclose(fichier);
}

void generer_dot(int **mat)
{
	FILE *fichier=fopen("rer_pont.dot","w");
	int x,y;
	if(fichier==NULL)
	    {
	    	printf("Erreur d'ouvertur\n");
	    	exit(1);
	    }
	 fprintf(fichier,"Graph G {\nsize=\"50.0,50.0\";\nratio=\"fill\";\nnode [height=1,width=1]\n");
    
	 for(x=0;x<NB_STATION;++x)
	 {
	 	for(y=x;y<NB_STATION;++y)
		{
			if((mat[x][y]>0) && (mat[x][y]!=2))
		    	{
		      		fprintf(fichier,"%d -- %d\n",x,y);
		    	}
			if(mat[x][y]==2)
			{
				fprintf(fichier,"%d -- %d [fontsize=40, style=bold, color=red]\n",x,y);
			}
		}
	    }

	fprintf(fichier,"}");
	fclose(fichier);
}


int main(int argc, char * argv[]){
	char dot[]="rer.dot";
	char svg[]="rer";
	char extension[15]="svg";
	char dot2[]="rer_pont.dot";
	char svg2[]="rer_pont";
	generer_image(dot,svg,extension);
	int i;
	int j;
	char ** nom_station=malloc(NB_STATION * sizeof(char*));;
	int ** matrice = malloc(NB_STATION*sizeof(int*));
	for(i=0;i<NB_STATION;i++)
	{
	    nom_station[i]=malloc(NB_CHAR * sizeof(char));
	    matrice[i]=malloc(NB_STATION*sizeof(int));

	}
	creation_nom_station(nom_station, "rer.txt");
	creation_matrice(matrice, "rer.dot");
	cherche_pont(matrice,nom_station);
	cherche_double_pont(matrice,nom_station);
	generer_dot(matrice);
	generer_image(dot2,svg2,extension);	
	for(i=0;i<NB_STATION;i++)
	{
		free(matrice[i]);
		free(nom_station[i]);
	}
	free(nom_station);
	free(matrice);
    return 0;	
}
