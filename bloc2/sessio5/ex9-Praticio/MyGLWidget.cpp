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
  initVariables();
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)

  carregaShaders();
  createBuffers();
}


//**************F.PREFEFINIDES**************//

void MyGLWidget::paintGL ()
{

  // 1. Esborrem el frame-buffer (PER DEFECTE)
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  calcCapsaContenidora(m,maxCapsa,minCapsa);
  radi = calculRadi(maxCapsa,minCapsa);

  // 2. Project+View
  init_camera();

  //**OBJECTE 1 HOMER********//
  paintModel(m,VAO_Homer);
  //**OBJECTE 2 TERRA********//
  //paintTerra(VAO_Terra);

}

void MyGLWidget::resizeGL (int w, int h)
{

    glViewport(0, 0, w, h);
    ra =  float(width())/float(height());
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
      rotate += M_PI/8;
      modelTransformRot ();
      break;

    }
    default: event->ignore(); break;
  }
  update();
}


//***********INICIALITZACIONS*****************//

void MyGLWidget::initVariables(){
    scale = 1.0f;
    rotate = 0.0f;
    ra = 1.0f;
    angle = (float) ((M_PI)/4.0);
    znear = 1.8f;
    zfar = 1.0f;
}

void MyGLWidget::init_camera(){

    /**CONFIG PROJECCIO **/

    FOV = (float)M_PI/2.0f;
    angle = (float) ((M_PI)/4.0);
    ra = 1.0f;
    znear = 1.5;
    zfar = (maxCapsa.z-minCapsa.z)+1.5;
    //Carreguem la projeccio del model
    projectTransform();

    /**CONFIG VIEW**/

    OBS = glm::vec3(0,0,-radi*2);
    VRP = glm::vec3(0,0,0);
    UP = glm::vec3(0,1,0);

    //Carreguem la view del model
    viewTransform();
}


//********MATRIUS TRANSFORMACIONS***********//

void MyGLWidget::modelTransform ()
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);

  calcCapsaContenidora(m,maxCapsa,minCapsa);
  glm::vec3 capsa = calcCentreCapsa(maxCapsa,minCapsa);

  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::translate(transform,capsa);

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformRot()
{

  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  glm::vec3 rot = glm::vec3(0.,1.0,0.);

  calcCapsaContenidora(m,maxCapsa,minCapsa);
  glm::vec3 capsa = calcCentreCapsa(maxCapsa,minCapsa);


  transform = glm::rotate(transform,rotate,rot);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::translate(transform,capsa);


  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform () {

    FOV = 2*atan((maxCapsa.y-minCapsa.y/3));

    resizeAngle(); //Per quan es modifica el ra

    znear = 1.5;
    zfar = 1.5 + (maxCapsa.z - minCapsa.z);

    glm::mat4 Proj = glm::perspective (FOV,ra,znear,zfar);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);

}

void MyGLWidget::viewTransform ()
{
  // Matriu de transformació de model
     // glm::lookAt (OBS, VRP, UP) : Perspective

    radi = calculRadi(maxCapsa,minCapsa);
    centreCapsa = calcCentreCapsa(maxCapsa,minCapsa);

    double distancia = calculDistancia(centreCapsa,maxCapsa);

    glm::mat4 View = glm::mat4(1.0f);
    View = glm::translate(View,glm::vec3(0.,0.,-distancia+1.5));
    View = glm::rotate(View, float(M_PI/2.0),glm::vec3(1,0,0));
    View = glm::rotate(View, float(-M_PI/2.0),glm::vec3(0,1,0));
    View = glm::translate(View, glm::vec3(-VRP.x,-VRP.y,-VRP.z));

    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}


//*********PINTAR OBJECTES**********//
void MyGLWidget::paintModel(const Model &model, const GLuint &VAO) {

    //1. Transformacio del model
    modelTransformRot();

    //2. Pintar Model
    // Activem el VAO
    glBindVertexArray (VAO);
    // Pintem
    glDrawArrays (GL_TRIANGLES, 0, model.faces().size () * 3);
    // Descativem
    glBindVertexArray (0);

}

void MyGLWidget::paintTerra(const GLuint &VAO) {
    //Tranformacio del model  (nomes es torna a aplicar el model
    //                        transform que es l'unic que volem
    //                        diferent)
    modelTransform ();

    glBindVertexArray (VAO);
    // pintem
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray (0);
}



//*********************CALCULS DELS MODELS****************************************//
float MyGLWidget::calculDistancia(glm::vec3 ini, glm::vec3 fi) {
    float x = fi.x- ini.x;
    float y = fi.y- ini.y;
    float z = fi.z- ini.z;
    float d = sqrt(x*x+y*y+z*z);
    std::cout << "sqrt("<<x<<"²+"<<y<<"²+"<<z<<")="<<d<<std::endl;
    return d;
}

//Es modifica nomes si s'ha modificat ra
void MyGLWidget::resizeAngle(){

    if(ra>=1) FOV = angle*2;
    else {
        angle = atan(tan(angle)/ra);
        FOV = angle*2;
    }
}


float MyGLWidget::calculRadi(const glm::vec3 &max,const glm::vec3 &min) {
    float x=max.x-min.x;
    float y=max.y-min.y;
    float z=max.z-min.z;
    float r = sqrt(x*x+y*y+z*z)/2;

    return r;
}

void MyGLWidget::calcCapsaContenidora(const Model &model,glm::vec3& max,glm::vec3& min)
{
    //calcul max i min

    min.x = max.x = model.vertices()[0];
    min.y = max.y = model.vertices()[1];
    min.z = max.z = model.vertices()[2];

    for(unsigned int i = 3;i<model.vertices().size();i+=3) {
        double x = model.vertices()[i];
        double y = model.vertices()[i+1];
        double z = model.vertices()[i+2];

        if(x<min.x) min.x = x;
        else if (x>max.x) max.x = x;

        if(y<min.y) min.y = y;
        else if (y>max.y) max.y = y;

        if(z<min.z) min.z = z;
        else if (z>max.z) max.z = z;
    }
}

glm::vec3 MyGLWidget::calcCentreCapsa(const glm::vec3 &max, const glm::vec3 &min) {

    float cxModel = -(max.x+min.x)/2;
    float cyModel = -(max.y+min.y)/2;
    float czModel = -(max.z+min.z)/2;
    std::cerr <<  cxModel << "" << cyModel << "" << czModel << std::endl;
    return glm::vec3(cxModel, cyModel, czModel);
}


//*******CARREGA DE INFORMACIO***********//
void MyGLWidget::createBuffers ()
{

  //*********************************************************************************//
  //*******************************MODEL*********************************************//
  //*********************************************************************************//

   m.load ("./models/Patricio.obj");
   calcCapsaContenidora(m,maxCapsa,minCapsa);

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

