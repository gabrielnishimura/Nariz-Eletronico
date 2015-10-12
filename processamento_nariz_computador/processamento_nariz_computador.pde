import processing.serial.*;
Serial arduino;
PrintWriter output;
int estado = 0;
String input="", question="";
String file_name = "";
PFont f;

/** Funcoes dentro do setup serao executadas antes de tudo */
void setup() {
  /** Cria a tela do programa em Processing, de tamanho 500x100pixels, com a cor de fundo branca */
  size(600, 300);
  fill(0);
  f = createFont("Arial",16,true);
  
  /** Inicializar canal serial com arduino. Verificar se a conexao ja nao esta aberta em outro programa */
  //println(Serial.list()); // Caso seja necessario verificar quais sao as portas do sistema
  arduino = new Serial( this, Serial.list()[0], 9600 ); // fechar Monitor Serial antes de 
  // inicializar o processing e apertar o botao RESET
  question = "Escreva o nome do arquivo";
  printText(question, input);
}

/** Semelhante ao loop do arduino, onde a tela e` atualizada */
void draw() {
  if (arduino.available() > 0 ) {
    String value = arduino.readString();
    if (estado == 1) {
        if (value.indexOf(">>>>>") != -1) {
            estado = 2;
            printText("O programa ira guardar os dados no arquivo agora. Ele fechara quando acabar de gravar", "");
        }
        question+=value;
        printText(question,input);
    } else {
        if (value.indexOf("&") != -1) {
            output.flush();  // Writes the remaining data to the file
            output.close();  // Finishes the file
            exit();  // Stops the program
        }
        //println("Recebido valor do arduino '" + value + "', guardado no arquivo data.txt");
        output.print( value );
    }
  }
}

/** Chamada quando uma tecla e` apertada. */
void keyPressed() {
    if (key==RETURN || key==ENTER) { // caso a tecla enter seja apertada
        if (estado == 0) { // estado 0, esperando para nomear o arquivo
            file_name = input;
            question = "O sistema salvara um arquivo com nome '" + file_name + "'.\n";
            printText(question, "");
            output = createWriter( input ); // cria arquivo com nome "data.txt" para escrever
            arduino.write( "1234" );
            estado = 1;
        } else { // estado 1, arquivo foi nomeado
            arduino.write(input);
            println( "Enviado dado para porta serial" );
            question = "";
        }
        input=""; // limpar 
    } // else if
    else if (key==BACKSPACE) {
        if (input.length() > 0)
            input = input.substring(0, input.length() - 1);
    }
    else if (key==ESC) {
        output.flush();  // Writes the remaining data to the file
        output.close();  // Finishes the file
        exit();  // Stops the program
    }
    else {
      input = input + key; // criando a string a ser mandada para o arduino, caractere por caractere
    } // else
    // Atualiza o output atual
    printText(question,input);
}

void printText(String q, String a) {
    background(255);
    textFont(f);       
    fill(0);
    textAlign(CENTER);
    text(q,width/2,50);
    if (a != "")
        a = ">" + a;
    text(a,width/2,200);
}