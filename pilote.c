// Problème avec le ctrl Z lié au bash ? Après un crtl Z il faut réactiver le processus avec fg 
// ou ne pas utiliser le make -> juste le .pilote
// Problème avec le ctrl X lié au bash -> sur nos machines j'ai laissé le signal SIGQUIT avec ^\ et pas ^X

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid;
enum mode {ARRET, LENT, RAPIDE, INTERMITTENT};
enum mode mode = ARRET; // plus lisible avec un type énuméré qu'avec des int
int vitesse_intermittent = 3;

void lancerSimul(){
    execl("./simul", "simul", NULL);
}

// Handlers pour chaque signal
void handleSIGALRM(int signal){
    printf("Received Alarme\n");
    kill(pid,SIGUSR1);
    alarm(vitesse_intermittent);
}

void handleSIGINT(int signal){ // M/A -> Ctrl-C
    if(mode == ARRET){// C'était à l'arrêt
        printf("Received SIGINT\n");
        mode = LENT; //lent
        kill(pid,SIGHUP); //continu lent
    }
    else{ // C'était en marche, on arrête
        printf("Received SIGINT\n");
        mode = ARRET; //arret
        alarm(0);//stop l'alarme
        kill(pid,SIGUSR2); //arret
    }
}

void handleSIGQUIT(int signal){ // + -> Ctrl-X est ce qu'il faut changer ?
    if(mode == LENT){ //On était en mode lent, on passe en rapide
        printf("Received SIGQUIT mode 1\n");
        mode = RAPIDE; // passer en rapide
        kill(pid,SIGPIPE);
    }
    else if(mode==INTERMITTENT){ //c'était en intermittent
        // alarm(N-1)
        printf("Received SIGQUIT mode 3\n");
        if (vitesse_intermittent > 3){ 
            vitesse_intermittent--;
            handleSIGALRM(0); //il set l'alarme
        }
        else{
            mode = LENT; //lent
            alarm(0); // désactive l'alarme
            kill(pid,SIGHUP); //continu lent
        }
    }
}

void handleSIGTSTP(int signal){ // - -> Ctrl-Z
    if(mode == LENT){ //On était en mode lent, on passe en intermittent
        printf("Received SIGTSTP mode 1\n");
        mode = INTERMITTENT; // passer en intermittent
        kill(pid,SIGUSR2); // ARRET
        handleSIGALRM(0); //il set l'alarme
    }
    else if (mode == RAPIDE){ // On était en mode rapide, on passe en lent
        printf("Received SIGTSTP mode 2\n");
        mode = LENT; // passer en lent
        kill(pid,SIGHUP);
    }
    else if(mode == INTERMITTENT){ //c'était en intermittent
        // alarm(N+1) 
        printf("Received SIGTSTP mode 3\n");
        if (vitesse_intermittent < 10){ 
            vitesse_intermittent++; 
            handleSIGALRM(0); //il set l'alarme
        }
        else{
            mode = ARRET; //arret
            alarm(0); // désactive l'alarme
            kill(pid,SIGTERM); //arret complet
        }
    }
}

int main(){
    pid = fork();

    if (pid == -1) {
        // Gestion de l'erreur de fork
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Code du fils
        lancerSimul();
    } else {

    struct sigaction sa={0};

    sigset_t mask;
    sigemptyset(&mask);

    sigemptyset(&sa.sa_mask);


    // Configuration des handlers
    sa.sa_flags = 0;

    sa.sa_handler = handleSIGINT;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handleSIGQUIT;
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = handleSIGTSTP;
    sigaction(SIGTSTP, &sa, NULL);

    sa.sa_handler = handleSIGALRM;
    sigaction(SIGALRM, &sa, NULL);

    printf("PID = %d\n", pid);
    printf("Contrôlez les essuie-glaces :\n");

    while (1) {
            if (mode == ARRET) {
                // Bloquer les signaux SIGTSTP et SIGQUIT lorsque le mode est ARRET
                sigaddset(&mask, SIGTSTP);
                sigaddset(&mask, SIGQUIT);
                sigprocmask(SIG_BLOCK, &mask, NULL);
            } else {
                // Débloquer les signaux SIGTSTP et SIGQUIT dans les autres modes
                sigaddset(&mask, SIGTSTP);
                sigaddset(&mask, SIGQUIT);
                sigprocmask(SIG_UNBLOCK, &mask, NULL);
            }
            pause();
        }
    }
    }