#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
  rotate = 0.0f;
  znear = 1.0f;
  zfar = 5.0f;
  escalat = 0.0f;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

//----------------------------------------------------------------
//-----------------FUNCIONS OPEN GL-------------------------------
//----------------------------------------------------------------

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
}

void MyGLWidget::paintGL ()
{

  // Esborrem el frame-buffer (PER DEFECTE)
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Inicialitzem variables de la camera
  init_camera();


  //__________________OBJECTE 2 TERRA________________________//
  // 1. Transform del model
  modelTransform ();

  glBindVertexArray (VAO_Terra);         //Activem
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Pintem
  glBindVertexArray (0);                 //Desactivem


  //____________________OBJECTE 1 HOMER_____________________//

  // 1.Model transf del model
  calculCapsaContenidora(m,maxCapsa,minCapsa);
  modelTransformRot();

  // 2. Pintar l'objecte
  glBindVertexArray (VAO_Model);                      //Activem
  glDrawArrays (GL_TRIANGLES, 0, m.faces().size()*3); //View
  glBindVertexArray (0);                              //Descativem




}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport(0, 0, w, h);
  resizeFOV();
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
-----FUNCIONS INICIALITZACIO-------------------------
//----------------------------------------------------------------

void MyGLWidget::ini

//----------------------------------------------------------------
//-----------t_camera(){


    //CALCULS NECESSARIS
    calculCapsaContenidora(m,maxCapsa,minCapsa);

    glm::vec3 centre = calculCentreCapsa(maxCapsa,minCapsa);
    float radi = calculDist(maxCapsa,centre);


    /**------CONFIG PROJECCIO--------**/

    FOV = (float)M_PI/3.0;
    angle = FOV/2;
    //ra = 1.0f;
    znear = 0.1f;
    zfar = 10.0f*radi;
    //Carreguem la projeccio del model
    projectTransform();

    /**--------CONFIG VIEW-----------**/

    //Obs respecte objecte
    OBS.x = 0;
    OBS.y = 0;
    OBS.z = radi*2;

    //Centre resperte objecte
    VRP.z = 0;
    VRP.y = centre.y;
    VRP.x = 0;

    //UP
    UP = glm::vec3(0.,1.,0.);

    //Carreguem la view del model
    viewTransform();
}


//----------------------------------------------------------------
//-------------FUNCIONS CONFIG OPEN GL----------------------------
//----------------------------------------------------------------

void MyGLWidget::modelTransform ()
{


  //calculCapsaContenidora(m,maxCapsa,minCapsa);
  //glm::vec3 centre = calculCentreCapsa(maxCapsa,minCapsa);

  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  //transform = glm::translate(transform,-centre);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformRot ()
{
  // Matriu de transformació de model

  glm::vec3 rot(0);
  rot[1] = 1; //Inicialitzo a 1
  calculCapsaContenidora(m,maxCapsa,minCapsa);
  glm::vec3 centre = calculCentreCapsa(maxCapsa,minCapsa);

  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::rotate(transform,rotate,rot);
  transform = glm::translate(transform,-centre);

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform () {


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


//----------------------------------------------------------------
//-------------FUNCIONS CALCUL------------------------------------
//----------------------------------------------------------------

void MyGLWidget::resizeFOV(){

    ra = float(width())/float(height());

    if(ra>=1) FOV = angle*2;
    else {
        angle = atan(tan(angle)/ra);
        FOV = angle*2;
    }
}

void MyGLWidget::calculCapsaContenidora(const Model &m, glm::vec3 &max, glm::vec3 &min) {
    //calcul max i min

    // Càlcul capsa contenidora i valors transformacions inicials
     min.x = max.x = m.vertices()[0];
     min.y = max.y = m.vertices()[1];
     min.z = max.z = m.vertices()[2];
     for (unsigned int i = 3; i < m.vertices().size(); i+=3)
     {
       if (m.vertices()[i+0] < min.x) min.x = m.vertices()[i+0];
       if (m.vertices()[i+0] > max.x) max.x = m.vertices()[i+0];
       if (m.vertices()[i+1] < min.y) min.y = m.vertices()[i+1];
       if (m.vertices()[i+1] > max.y) max.y = m.vertices()[i+1];
       if (m.vertices()[i+2] < min.z) min.z = m.vertices()[i+2];
       if (m.vertices()[i+2] > max.z) max.z = m.vertices()[i+2];
   }

}

glm::vec3 MyGLWidget::calculCentreCapsa(const glm::vec3 &max,const glm::vec3 &min) {
    glm::vec3 centreCaixa;

    centreCaixa.x = (max.x - min.x)/2.0f;
    centreCaixa.y = (max.y - min.y)/2.0f;
    centreCaixa.z = (max.z - min.z)/2.0f;

    return centreCaixa;
}

float MyGLWidget::calculDist(const glm::vec3 &max,const glm::vec3 &min) {
     float x = max.x - min.x;
     float y = max.y - min.y;
     float z = max.z - min.z;

     float radi = (float) sqrt(x*x+y*y+z*z);

     std::cout << "sqrt("<<x<<"²+"<<y<<"²+"<<z<<")="<<radi<<std::endl;
     return radi;
}


//----------------------------------------------------------------
//------------FUNCIONS AUXILIARS----------------------------------
//----------------------------------------------------------------

void MyGLWidget::createBuffers ()
{

  //*********************************************************************************//
  //*******************************MODEL*********************************************//
  //*********************************************************************************//

   m.load ("./models/Patricio.obj");

  // 1. Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Model);
  glBindVertexArray(VAO_Model);

  // 2. Posicio
  glGenBuffers(1, &VBO_ModelPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_ModelPos);
  glBufferData (GL_ARRAY_BUFFER,sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW); // posició


  //3. Vertexs posicio:  Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  //4. Colors
  glGenBuffers(1, &VBO_ModelCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_ModelCol);
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

