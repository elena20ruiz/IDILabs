#include "MyGLWidget.h"

#include <iostream>


MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (1.0, 0.0, 0.6, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Activem l'Array a pintar 
  glBindVertexArray(VAO);
  // Pintem l'escena
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
  // Desactivem el VAO
  glBindVertexArray(0);

  //SEGON EXEMPLE
  glBindVertexArray(VAO2);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);


}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
}

void MyGLWidget::createBuffers ()
{
  glm::vec3 Vertices[3];  // Tres vèrtexs amb X, Y i Z
  Vertices[0] = glm::vec3(-1, -1, 0.0);
  Vertices[1] = glm::vec3(1, -1, 0.0);
  Vertices[2] = glm::vec3(0, 1, 0.0);

  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex	
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::modelTransform(){

    glm::mat4 TG(1.0);
    //TG = glm::translate(TG,glm::vec3(0.0,0.0,0.0));
    glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);

}



void MyGLWidget::keyPressEvent (QKeyEvent *e) {
    glm::mat4 TG(1.0);
    makeCurrent ();
        switch ( e->key() ) {
        //Moure slc
        case Qt::Key_S :
            scl += 0.1;
            glUniform1f (varLoc, scl);
            break;
        case Qt::Key_D :
            scl -= 0.1;
            glUniform1f (varLoc, scl);
            break;

        //Moure matriu transmormacio
        case Qt::Key_Left :

            TG = glm::translate(TG,glm::vec3(-0.1,0.0,0.0));
            glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);
            TG = glm::rotate(transLoc, 3.14/4, glm::vec3(0.0,0.0,1.0));
            glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);

            break;

        case Qt::Key_Right :

            TG = glm::translate(TG,glm::vec3(0.1,0.0,0.0));
            glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);
            TG = glm::rotate(transLoc, 3.14/4, glm::vec3(0.0,0.0,1.0));
            glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);
            break;

        case Qt::Key_Up :

            TG = glm::translate(TG,glm::vec3(0.0,0.1,0.0));
            glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);
            break;

        case Qt::Key_Down :
            TG = glm::translate(TG,glm::vec3(0.0,-0.1,0.0));
            glUniformMatrix4fv(transLoc,1,GL_FALSE, &TG[0][0]);
            break;

        default: e->ignore (); // propagar al pare
        }
        update ();
}



void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  varLoc = glGetUniformLocation(program->programId(), "val");
  scl = 0.5;
  glUniform1f (varLoc,scl);
  // scl variable que conté el valor que es vol per “val”

  transLoc = glGetUniformLocation(program->programId(), "TG");
  modelTransform(); //Inicialitzar matriu transf


}
