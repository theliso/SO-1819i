### a) O texto organiza o tratamento da excepção page fault pelo respectivo handler em cinco categorias distintas.
### Enumere-as fazendo uma descrição breve do seu significado e descrevendo o tratamento correspondente do
### page fault handler.

 * A página referenciada não está _committed_;


 * O acesso a uma página resultou numa violação de permissões;

Um exemplo desta categoria é quando tentamos fazer uma escrita numa variável que é _read-only_. Então no processo de tradução estamos a tentar aceder a uma zona de memória onde as permissões de acesso é de leitura.

 * Uma página marcada com _copy-on-write_ foi recentemente modificada;



 * Necessidade de crescer o _stack_;

Um exemplo desta categoria, é quando tentamos aceder a uma variável em stack que não está alocada, ou que ainda não foi alocada.

 * A página referenciada está _committed_, mas não está mapeada;



