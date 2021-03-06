#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
  rotate = 0.0f;
  ra = 1.0f;
  angle = 1.0f;
  znear = 1.0f;
  zfar = 1.8f;
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

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
}
/*
Un cop tenim tota la cp, FOV, ra, znear i zfar) en variables que s’inicialitzen en un m`etode ini camera (). Aquest
m`etode a m ́es d’inicialitzar aquestes variables far`a les crides necess`aries per a inicialitzar les
matrius (projectTransform () i viewTransform ()).*/

void MyGLWidget::init_camera(){

    /**CONFIG PROJECCIO **/

    FOV = (float)M_PI/2.0f;
    ra = 1.0f;
    znear = 0.4f;
    zfar = 3.0f;
    //Carreguem la projeccio del model
    projectTransform();

    /**CONFIG VIEW**/

    OBS = glm::vec3(0,0,1);
    VRP = glm::vec3(0,0,0);
    UP = glm::vec3(0,1,0);

    //Carreguem la view del model
    viewTransform();
}


void MyGLWidget::paintGL ()
{

  // Esborrem el frame-buffer (PER DEFECTE)
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //************OBJECTE 1 HOMER****************//

  // 1 .Project+View
  init_camera();

  // 2. Transformacio del model a pintar
  modelTransformRot();

  // 3. Pintem objectes
  // Activem el VAO per a pintar homer
  glBindVertexArray (VAO_Homer);

  //View
  glDrawArrays (GL_TRIANGLES, 0, m.faces().size()*3);

  //************OBJECTE 2 TERRA***************//


  //Tranformacio del model  (nomes es torna a aplicar el model
  //                        transform que es l'unic que volem
  //                        diferent)
  modelTransform ();

  glBindVertexArray (VAO_Terra);
  // pintem
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}


//********MATRIUS TRANSFORMACIONS*************//

void MyGLWidget::modelTransform ()
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  glm::vec3 rot(0);
  rot[1] = 1; //Inicialitzo a 1

  transform = glm::scale(transform, glm::vec3(scale));

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformRot ()
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  glm::vec3 rot(0);
  rot[1] = 1; //Inicialitzo a 1

  transform = glm::rotate(transform,rotate,rot);
  transform = transform*glm::scale(transform, glm::vec3(scale));


  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform () {

    ra = float(width())/float(height());

    if(ra < 1) { angle = atan(tan(float((M_PI)/4.0))/ra);}
    FOV = 2*angle;

    // glm::perspective (FOV en radians, ra window, znear, zfar)
    glm::mat4 Proj = glm::perspective (FOV,ra,znear,zfar);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);

}

void MyGLWidget::viewTransform ()
{
  // Matriu de transformació de model
  // glm::lookAt (OBS, VRP, UP)

    glm::mat4 View = glm::lookAt (OBS,VRP,UP);
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}



void MyGLWidget::resizeGL (int w, int h)
{

    glViewport(0, 0, w, h);

    projectTransform();

}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
  switch (event->key()) {
    makeCurrent();
    case Qt::Key_S: { // escalar a més gran
      scale += 0.05;
      modelTransform();
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      modelTransform();
      break;
    }
    case Qt::Key_R: {
      rotate += M_PI/4;
      modelTransformRot ();
      break;

    }
    default: event->ignore(); break;
  }
  update();
}




void MyGLWidget::createBuffers ()
{

  //*********************************************************************************//
  //*******************************MODEL*********************************************//
  //*********************************************************************************//

   m.load ("./models/HomerProves.obj");

  // 1. Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  // 2. Posicio
  glGenBuffers(1, &VBO_HomerPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerPos);
  glBufferData (GL_ARRAY_BUFFER,sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW); // posició


  //3. Vertexs posicio:  Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  //4. Colors
  glGenBuffers(1, &VBO_HomerCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerCol);
  glBufferData (GL_ARRAY_BUFFER,sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_matdiff(), GL_STATIC_DRAW); // color


  //5. Vertex colors: Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);


  //*********************************************************************************//
  //*******************************AUXILIARS*****************************************//
  //*********************************************************************************//

  // Dades del terra
  // Dos VBOs, un amb posició i l'altre amb color
  glm::vec3 posterra[4] = {
    glm::vec3(-1.0, -1.0, -1.0),
    glm::vec3(-1.0, -1.0, 1.0),
    glm::vec3(1.0, -1.0, -1.0),
    glm::vec3(1.0, -1.0, 1.0)
  };
  glm::vec3 colterra[4] = {
    glm::vec3(1,0,1),
    glm::vec3(1,0,1),
    glm::vec3(1,0,1),
    glm::vec3(1,0,1)
  };

  // 1. Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  // 2. Posicio
  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData (GL_ARRAY_BUFFER,sizeof(posterra), posterra, GL_STATIC_DRAW); // posició

  // 3. Vertexs posicio:  Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  //4. Colors
  glGenBuffers(1, &VBO_TerraCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
  glBufferData (GL_ARRAY_BUFFER,sizeof(colterra), colterra, GL_STATIC_DRAW); // color


  //5. Vertex colors: Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  //6. Tanquem
  glBindVertexArray (0);

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

  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");

  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");

  //Uniform projection
  projLoc = glGetUniformLocation(program->programId(), "proj");

  //Uniform view
  viewLoc = glGetUniformLocation(program->programId(), "view");

  glEnable(GL_DEPTH_TEST);


}

