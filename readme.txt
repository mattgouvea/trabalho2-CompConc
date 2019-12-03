O programa recebe argumentos pela linha de comando e retorna um arquivo .c que
o usuário pode compilar e executar diretamente.

Passos para a execução do programa:

	1. Compilar o programa principal.c, passando como argumentos na linha de comando:

		-número de leitores
		-número de escritores
		-quantidade de leituras
		-quantidade de escritas
		-nome do arquivo de log
		
		Após a execução, será gerado um arquivo com a extensão .c, com o mesmo nome do parâmetro log.
		Este arquivo gera um histórico de todas as operações executadas pelas threads, em forma de
		chamadas de função, e o valor da variável compartilhada em cada operação, dentro de um código 
		que foi usado como modelo, possibilitando o usuário de simplesmente compilar e executar
		esse log, para verificar se o programa executou corretamente.

	2. Compilar o código c gerado pelo programa principal, executar esse programa, e verificar a saída.
