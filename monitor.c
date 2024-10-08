#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){

    // talvez se n realizar troca de informações entre processos usar thread

    pid_t pid = fork();

    if(pid == -1){
        perror("fork falhou!"); // mensagem de erro relacionado ao erro relacionada a úlima chamada do sistema que falhou
        exit(EXIT_FAILURE); // isso aqui é padrão de escrita, deixar mais legível que deu erro, em vez de dar return 1
    }

    else if(pid == 0){ // processo filho
        //execlp -> n precisa fornecer o caminhO
        execlp(
            "sudo", // primeiro sudo -> o execlp vai procurar o comando no PATH
            "sudo", // faz a execução do sudo mesmo -> executar como adm o próximo comando
            "trace-cmd", "record", // gravar os seguintes comandos
            "-e", "sched_switch", // -e -> especifica um elemento a ser procurado, que nesse caso é as trocas de contexto
            "-e", "sched_wakeup", // aqui vai ser acordar os processos e ir p fila de prontos
            "-e", "sched_process_exit", // aqui vai ser p pegar o fim dos processos 
            "-o", "saida.monitoramento.dat",// - o vai especificar o arquivo de saída
            NULL // o último argumento precisa ser NULL pro comando entender que acabou a entrada
        );

        perror("o exec falhou!");
        exit(EXIT_FAILURE);
    }

    else{

        int status;
        waitpid(pid, &status, 0); // vamos esperar o processo filho terminar para podermos realizar a "tradução" das informações

        // o -i significa que estou oferencendo arquivos de entrada e o report é basicamente p traduzir p escrita normal
        execlp("sudo","sudo","trace-cmd","report","-i", "saida.monitoramento.dat", NULL);

        perror("deu erro no ultimo exec!");
        exit(EXIT_FAILURE);

    }

    exit(EXIT_SUCCESS); // o programa rodou perfeitamente!
}