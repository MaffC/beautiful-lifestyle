/*
 * hue 0.1
 * A very basic Philips Hue-like application using the PiGlow board.
 * requires wiringPi
 * Copyright 2014, Matthew Connelly.
 *
 * TODO:
 * Fix fade delaying
 * Maybe make argument parsing less of a hackish nightmare
 */

// includes {{{
#include "hue.h"
//}}}

// variable defs {{{
state light;
state update;
cmd hue;
//}}}

void argparse(int argc, char *argv[]) {//{{{
	int foffset=0;
	if((argc>1&&(strcmp(argv[1],"help")==0||strcmp(argv[1],"-h")==0||strcmp(argv[1],"--help")==0))||argc==1||(strcmp(argv[1],"fade")==0&&argc<=3)) {usage();exit(0);}
	if(strcmp(argv[1],"status")==0) {printstate();exit(0);}
	if(strcmp(argv[1],"fade")==0&&argc>2&&atoi(argv[2])==0&&strcmp(argv[2],"0")!=0&&strcmp(argv[2],"off")!=0) fatal("Invalid argument to option fade!");
	else if(strcmp(argv[1],"fade")==0&&argc>3) {hue.fade=atoi(argv[2]);foffset=2;}
	if(strcmp(argv[foffset+1],"on")==0) {hue.mode=1;return;} //we return because `on` does not take any other args
	if(strcmp(argv[foffset+1],"off")==0) {hue.mode=2;return;} //and here too
	//at this point the only other command left is `hue [opts] (colour) (brightness)` so..
	if(strcmp(argv[foffset+1],"red")==0) hue.colour=0;
	if(strcmp(argv[foffset+1],"orange")==0) hue.colour=1;
	if(strcmp(argv[foffset+1],"yellow")==0) hue.colour=2;
	if(strcmp(argv[foffset+1],"green")==0) hue.colour=3;
	if(strcmp(argv[foffset+1],"blue")==0) hue.colour=4;
	if(strcmp(argv[foffset+1],"white")==0) hue.colour=5;
	if(strcmp(argv[foffset+1],"all")==0) hue.colour=6;
	if(hue.colour>=0&&argc==foffset+3) {hue.mode=3;if(strcmp(argv[foffset+2],"off")==0||atoi(argv[foffset+2])==0) hue.brightness=0; else hue.brightness=atoi(argv[foffset+2]);}
	if(hue.brightness<0||hue.brightness>255) fatal("Specified brightness must be between 0 and 255!");
	if(!hue.mode) {usage();exit(0);}
} //}}}
void setup(bool clear) {//{{{
	light=(state){.on=0};
	hue=(cmd){.mode=0,.colour=-1,.fade=4};
	wiringPiSetup();
	piGlowSetup(clear);
	int res=readstate();
	if(res==2) echo("Created state file ~/.huestate");
	else if(res<1) warn("Couldn't read ~/.huestate, you may need to check permissions on this file or delete it.");
	update=light;
}//}}}
int gammaCorrect(int pwm) {//{{{
	if(pwm<0) return 0;
	if(pwm>255) return 255;
	//I sincerely apologise for this.
	//gamma correction array pulled from https://raw.githubusercontent.com/benleb/PyGlow/master/PyGlow.py
	//I couldn't figure out a better way to do it.
	int gamma[256] = {0,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,
			2,2,2,3,3,3,3,3,
			3,3,3,3,3,3,3,3,
			4,4,4,4,4,4,4,4,
			4,4,4,5,5,5,5,5,
			5,5,5,6,6,6,6,6,
			6,6,7,7,7,7,7,7,
			8,8,8,8,8,8,9,9,
			9,9,10,10,10,10,10,11,
			11,11,11,12,12,12,13,13,
			13,13,14,14,14,15,15,15,
			16,16,16,17,17,18,18,18,
			19,19,20,20,20,21,21,22,
			22,23,23,24,24,25,26,26,
			27,27,28,29,29,30,31,31,
			32,33,33,34,35,36,36,37,
			38,39,40,41,42,42,43,44,
			45,46,47,48,50,51,52,53,
			54,55,57,58,59,60,62,63,
			64,66,67,69,70,72,74,75,
			77,79,80,82,84,86,88,90,
			91,94,96,98,100,102,104,107,
			109,111,114,116,119,122,124,127,
			130,133,136,139,142,145,148,151,
			155,158,161,165,169,172,176,180,
			184,188,192,196,201,205,210,214,
			219,224,229,234,239,244,250,255};
	return gamma[pwm];
}//}}}
int percentage(int intensity) {//{{{
	return (int)(((float)intensity/255.0)*100.0);
}//}}}
void echo(char *line) {//{{{
	printf("%s\n",line);
}//}}}
void warn(char *msg) {//{{{
	fprintf(stderr,"%s\n",msg);
}//}}}
void fatal(char *msg) {//{{{
	fprintf(stderr,"%s\n",msg);
	exit(1);
}//}}}
void usage(void) {//{{{
	echo("hue usage:");
	echo("hue [options] [command]");
	echo("\nCommands:");
	echo("hue [opts] off: Turn lights off.");
	echo("hue [opts] on: Turn lights on using previous configuration.");
	echo("hue [opts] (colour) (brightness): Turn lights on and set them to the colour and brightness given.");
	echo("\nOptions:");
	echo("fade (time in whole seconds): Controls the duration of the fade on or off, 0 or 'off' for no fade.");
	echo("\nColours:");
	echo("red, orange, yellow, green, blue, white, all");
	echo("\nUsage examples:");
	echo("hue help: show this help text");
	echo("hue fade 5 off: fade the light off over a five-second duration");
}//}}}
void printstate(void) {//{{{
	printf("Light %s",light.on? "on" : "off");
	printf("\nRed: %i%%, Orange: %i%%, Yellow: %i%%, Green: %i%%, Blue: %i%%, White: %i%%\n",
			percentage(light.led[0]),percentage(light.led[1]),percentage(light.led[2]),
			percentage(light.led[3]),percentage(light.led[4]),percentage(light.led[5]));
}//}}}
void process(void) {//{{{
	if(hue.mode==1) {update=light;memset(light.led,0,sizeof(light.led));}
	if(hue.mode==2) memset(&update.led,0,sizeof(update.led));
	if(hue.mode==3) for(int i=0;i<6;i++) if(i==hue.colour||hue.colour==6) update.led[i]=hue.brightness;
}//}}}
int readstate(void) {//{{{
	FILE *fh = fopen("/home/pi/.huestate", "r");
	if(fh==NULL&&errno==ENOENT) {
		if(writestate()) return 2; //we write a blank state file
		else { warn("Couldn't create ~/.huestate after getting ENOENT on open");return 0;}
	} else if (fh<0) return -1;
	fscanf(fh,"%i,",&light.on);
	if(light.on<0||light.on>1) light.on=0;
	for(int c=0;c<6;c++) {
		fscanf(fh,"%i,",&light.led[c]);
		if(light.led[c]<0) light.led[c]=0;
		else if(light.led[c]>255) light.led[c]=255;
	}
	fclose(fh);
	return 1;
}//}}}
int writestate(void) {//{{{
	FILE *fh = fopen("/home/pi/.huestate", "w");
	if(fh==NULL) { fprintf(stderr,"Couldn't open .huestate for writing! errno=%i\n",errno);return 0;}
	fprintf(fh,"%i,",light.on);
	for(int c=0;c<6;c++) fprintf(fh,"%i,",light.led[c]);
	fclose(fh);
	return 1;
}//}}}
void fade(void) {//{{{
	if(hue.fade==0) return;
	int steps=hue.fade*20;
	const struct timespec sleep={0,((hue.fade*10000000)/steps)};
	int csteps[2][6]={0}; //csteps[0][0-5] indicate fade direction, [1][0-5] indicate how much to incrememnt brightness by each step
	for(int c=0;c<6;c++) {
		if(light.led[c]!=update.led[c]) {
			csteps[0][c]=(light.led[c]>update.led[c])? -1 : 1;
			csteps[1][c]=((update.led[c]-light.led[c])*csteps[0][c])/steps;
			if(csteps[1][c]==0) csteps[1][c]=1;
		} else csteps[0][c]=0;
	}
#ifdef DEBUG
	printf("steps=%i, sleep=%li, fadesteps: r=%i,o=%i,y=%i,g=%i,b=%i,w=%i\n",steps,(long)sleep.tv_nsec,csteps[1][0],csteps[1][1],csteps[1][2],csteps[1][3],csteps[1][4],csteps[1][5]);
#endif
	while(true) {
		int skipped=0;
		for(int c=0;c<6;c++) {
			if(csteps[0][c]!=0&&light.led[c]==update.led[c]) csteps[0][c]=0;
			if(csteps[0][c]==0) {skipped++;continue;}
			skipped=0;
			light.led[c]+=csteps[1][c]*csteps[0][c];
			if((light.led[c]>=update.led[c]&&csteps[0][c]==1)||(light.led[c]<=update.led[c]&&csteps[0][c]==-1)) {
				csteps[0][c]=0;
				light.led[c]=update.led[c];
			}
		}
		refreshleds(false);
		if(skipped==6) break;
		fflush(stdout);
		nanosleep(&sleep, NULL);
	}
	light=update;
	refreshleds(false);
}//}}}
void refreshleds(bool updatedstate) {//{{{
	for(int led=0;led<6;led++) piGlowRing(led,gammaCorrect(updatedstate? update.led[led] : light.led[led]));
}//}}}

int main(int argc, char *argv []) {//{{{1
	setup(false);//we run setup after argparsing so shit don't get fucked with needlessly
	argparse(argc,argv);//argparse does not return a value indicating if opts were parsed successfully because tbh if argparsing fails the thing should crash and burn
	process();
	switch(hue.mode) {//{{{2
		case 1:
			if(light.on) fatal("The light is already on.");
			//we turn the light on
			if(hue.fade==0) refreshleds(true);
			else fade();
			light.on=1;
			writestate();
			break;
		case 2:
			if(!light.on) fatal("The light is already off.");
			light.on=0;
			//we turn the light off
			writestate();
			if(hue.fade==0) refreshleds(true);
			else fade();
			break;
		case 3:
			if(!light.on) fatal("The light is not on.");
			//we make with the rainbows
			if(hue.fade==0) refreshleds(true);
			else fade();
			writestate();
			break;
		default:
			fatal("Oops, something went incredibly wrong.");
	}//}}}
	return 0;
}//}}}
