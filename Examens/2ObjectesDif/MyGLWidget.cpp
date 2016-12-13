#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  angleY = angleX = 0.0;
  perspectiva = true;
  DoingInteractive = NONE;
  radiEsc = sqrt(5);

  //ILUMINACIO
  vec_posFocus = glm::vec3(0.8, 0.8, 0.8);
  vec_colFocus = glm::vec3(1, 1, 1);


  //NOU------------------------------------
    zoom = 0.0f;
  //----------------------------------------

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
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  createBuffers();

  //FUNCIONS PROPIES______________
  distanciaMaximaEscena();
  refreshColFocus();
  refreshPosFocus();

  projectTransform ();
  viewTransform ();
}

void MyGLWidget::init_camera(){

    FOV = (float)M_PI/3.0f;
    ra = 1.0f;
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  init_camera();
  // Activem el VAO per a pintar el terra 
  glBindVertexArray (VAO_Terra);

  modelTransformTerra ();

  // pintem
  glDrawArrays(GL_TRIANGLES, 0, 12);

  // Activem el VAO per a pintar el Patricio
  glBindVertexArray (VAO_Caps);

  modelTransformCaps1 ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, caps.faces().size()*3);
  

  // Activem el VAO per a pintar el Patricio
  glBindVertexArray (VAO_Caps2);

  modelTransformCaps2 ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, caps2.faces().size()*3);




  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h) 
{
  projectTransform();
  glViewport(0, 0, w, h);

}

void MyGLWidget::createBuffers ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  caps.load("./models/cow.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel ();

  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Caps);
  glBindVertexArray(VAO_Caps);

  // Creació dels buffers del model patr
  // Buffer de posicions
  glGenBuffers(1, &VBO_CapsPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CapsPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps.faces().size()*3*3, caps.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glGenBuffers(1, &VBO_CapsNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CapsNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps.faces().size()*3*3, caps.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_CapsMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CapsMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps.faces().size()*3*3, caps.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_CapsMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CapsMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps.faces().size()*3*3, caps.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_CapsMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CapsMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps.faces().size()*3*3, caps.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_CapsMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CapsMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps.faces().size()*3, caps.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);


  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  caps2.load("./models/Patricio.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel2 ();

  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Caps2);
  glBindVertexArray(VAO_Caps2);

  // Creació dels buffers del model patr
  // Buffer de posicions
  glGenBuffers(1, &VBO_Caps2Pos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Caps2Pos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps2.faces().size()*3*3, caps2.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glGenBuffers(1, &VBO_Caps2Norm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Caps2Norm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps2.faces().size()*3*3, caps2.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_Caps2Matamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Caps2Matamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps2.faces().size()*3*3, caps2.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_Caps2Matdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Caps2Matdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps2.faces().size()*3*3, caps2.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_Caps2Matspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Caps2Matspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps2.faces().size()*3*3, caps2.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_Caps2Matshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Caps2Matshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*caps2.faces().size()*3, caps2.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);



  // Dades del terra
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[12] = {
	glm::vec3(-2.0, -1.0, 2.0),
	glm::vec3(2.0, -1.0, 2.0),
	glm::vec3(-2.0, -1.0, -2.0),
	glm::vec3(-2.0, -1.0, -2.0),
	glm::vec3(2.0, -1.0, 2.0),
	glm::vec3(2.0, -1.0, -2.0),
	glm::vec3(-2.0, -1.0, -2.0),
	glm::vec3(2.0, -1.0, -2.0),
	glm::vec3(-2.0, 1.0, -2.0),
	glm::vec3(-2.0, 1.0, -2.0),
	glm::vec3(2.0, -1.0, -2.0),
	glm::vec3(2.0, 1.0, -2.0)
  }; 

  //________________________________________________________________
  //___________________CALCUL MAX I MIN TERRA_______________________

  maxTerra.x = minTerra.x = posterra[0].x;
  maxTerra.y = minTerra.y = posterra[0].y;
  maxTerra.z = minTerra.z = posterra[0].z;

  for (unsigned int i = 0; i < 12; i+=1)
  {
      if (posterra[i].x < minTerra.x)
        minTerra.x = posterra[i].x;
      if (posterra[i].x > maxTerra.x)
        maxTerra.x = posterra[i].x;
      if (posterra[i].y < minTerra.y)
        minTerra.y = posterra[i].y;
      if (posterra[i].y > maxTerra.y)
        maxTerra.y = posterra[i].y;
      if (posterra[i].z < minTerra.z)
        minTerra.z = posterra[i].z;
      if (posterra[i].z > maxTerra.z)
        maxTerra.z = posterra[i].z;
  }
  centreTerra.x = (minTerra.x+maxTerra.x)/2.0;
  centreTerra.y = (minTerra.y+maxTerra.y)/2.0;
  centreTerra.z = (minTerra.z+maxTerra.z)/2.0;

   //________________________________________________________
   //_______________FI CALCUL________________________________





  // VBO amb la normal de cada vèrtex
  glm::vec3 norm1 (0,1,0);
  glm::vec3 norm2 (0,0,1);
  glm::vec3 normterra[12] = {
	norm1, norm1, norm1, norm1, norm1, norm1, // la normal (0,1,0) per als primers dos triangles
	norm2, norm2, norm2, norm2, norm2, norm2  // la normal (0,0,1) per als dos últims triangles
  };

  // Definim el material del terra
  glm::vec3 amb(0.2,0,0.2);
  glm::vec3 diff(0.8,0,0.8);
  glm::vec3 spec(0,0,0);
  float shin = 100;

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matambterra[12] = {
	amb, amb, amb, amb, amb, amb, amb, amb, amb, amb, amb, amb
  };
  glm::vec3 matdiffterra[12] = {
	diff, diff, diff, diff, diff, diff, diff, diff, diff, diff, diff, diff
  };
  glm::vec3 matspecterra[12] = {
	spec, spec, spec, spec, spec, spec, spec, spec, spec, spec, spec, spec
  };
  float matshinterra[12] = {
	shin, shin, shin, shin, shin, shin, shin, shin, shin, shin, shin, shin
  };

// Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_TerraMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matambterra), matambterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_TerraMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffterra), matdiffterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_TerraMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspecterra), matspecterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_TerraMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshinterra), matshinterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
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
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");

  //COLOR
  posFocus = glGetUniformLocation (program->programId(), "posFocus");
  colFocus = glGetUniformLocation (program->programId(), "colFocus");


}

void MyGLWidget::modelTransformCaps1 ()
{
  glm::vec3 rot = glm::vec3(1.,0.,0.);

  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG,glm::vec3(1.,-1+(centreCaps.y)/2,0));
  TG = glm::rotate(TG,(float)(-M_PI/2.0),rot);
  TG = glm::scale(TG, glm::vec3(escala/2, escala/2, escala/2));
  TG = glm::translate(TG, -centreCaps);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformCaps2 ()
{
  glm::vec3 rot;
  rot[2] = 1;

  glm::mat4 TG(1.f);  // Matriu de transformació

  TG = glm::translate(TG,glm::vec3(1.,-0.5+(centreCaps.y)/2,0.));
  TG = glm::rotate(TG,(float) M_PI,rot);
  TG = glm::scale(TG, glm::vec3(escala2, escala2, escala2));
  TG = glm::translate(TG, -centreCaps2);

  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG,glm::vec3(0.,-1+(centreCaps.y)/2,0.));
  TG = glm::translate(TG,glm::vec3(0.,-minTerra.y,0));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform ()
{
  //------------------------Per optimitzar viewport
  float angle = asin(radiEsc/(radiEsc*2))+zoom;

  //------------------------Per resize--------------
  ra = float(width())/float(height());

  if (ra < 1) FOV = atan(tan(angle*2)/ra);
  else FOV = angle*2;

  //------------------------------------------------






  glm::mat4 Proj;  // Matriu de projecció
  if (perspectiva)
    Proj = glm::perspective(FOV, ra, radiEsc, 3.0f*radiEsc);
  else
    Proj = glm::ortho(-radiEsc, radiEsc, -radiEsc, radiEsc, radiEsc, 3.0f*radiEsc);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{

  glm::vec3 VRP = glm::vec3(centreTerra.x, (centreTerra.y*2) ,centreTerra.z);
  float distancia = (1.5)*radiEsc;

  glm::mat4 View;  // Matriu de posició i orientació
  View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -distancia));
  View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
  View = glm::translate(View, -VRP);

  vec_posFocus = glm::vec3((VRP.x+distancia)*angleX*(-angleY),(VRP.y+distancia)*angleX*(-angleY),(VRP.z+distancia)*angleX*(-angleY));
  refreshPosFocus();

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::calculaCapsaModel ()
{
  // Càlcul capsa contenidora i valors transformacions inicials
  minCapsa.x = maxCapsa.x = caps.vertices()[0];
  minCapsa.y  = maxCapsa.y= caps.vertices()[1];
  minCapsa.z  = maxCapsa.z = caps.vertices()[2];
  for (unsigned int i = 3; i < caps.vertices().size(); i+=3)
  {
    if (caps.vertices()[i+0] < minCapsa.x)
      minCapsa.x = caps.vertices()[i+0];
    if (caps.vertices()[i+0] > maxCapsa.x)
      maxCapsa.x = caps.vertices()[i+0];
    if (caps.vertices()[i+1] < minCapsa.y)
      minCapsa.y = caps.vertices()[i+1];
    if (caps.vertices()[i+1] > maxCapsa.y)
      maxCapsa.y = caps.vertices()[i+1];
    if (caps.vertices()[i+2] < minCapsa.z)
      minCapsa.z = caps.vertices()[i+2];
    if (caps.vertices()[i+2] > maxCapsa.z)
      maxCapsa.z = caps.vertices()[i+2];
  }
  escala = 1/(maxCapsa.y - minCapsa.y);
  centreCaps[0] = (minCapsa.x+maxCapsa.x)/2.0;
  centreCaps[1] = (minCapsa.y+maxCapsa.y)/2.0;
  centreCaps[2] = (minCapsa.z+maxCapsa.z)/2.0;
}

void MyGLWidget::calculaCapsaModel2()
{
  // Càlcul capsa contenidora i valors transformacions inicials
  minCapsa2.x = maxCapsa2.x = caps2.vertices()[0];
  minCapsa2.y  = maxCapsa2.y= caps2.vertices()[1];
  minCapsa2.z  = maxCapsa2.z = caps2.vertices()[2];
  for (unsigned int i = 3; i < caps2.vertices().size(); i+=3)
  {
    if (caps2.vertices()[i+0] < minCapsa2.x)
      minCapsa2.x = caps2.vertices()[i+0];
    if (caps2.vertices()[i+0] > maxCapsa2.x)
      maxCapsa2.x = caps2.vertices()[i+0];
    if (caps2.vertices()[i+1] < minCapsa2.y)
      minCapsa2.y = caps2.vertices()[i+1];
    if (caps2.vertices()[i+1] > maxCapsa2.y)
      maxCapsa2.y = caps2.vertices()[i+1];
    if (caps2.vertices()[i+2] < minCapsa2.z)
      minCapsa2.z = caps2.vertices()[i+2];
    if (caps.vertices()[i+2] > maxCapsa2.z)
      maxCapsa2.z = caps2.vertices()[i+2];
  }
  escala2 = 0.25/(maxCapsa2.y - minCapsa2.y);
  centreCaps2[0] = (minCapsa2.x+maxCapsa2.x)/2.0;
  centreCaps2[1] = (minCapsa2.y+maxCapsa2.y)/2.0;
  centreCaps2[2] = (minCapsa2.z+maxCapsa2.z)/2.0;
}


//*********************************************************//
//**************FUNCIONS AUXILIARS************************//


float MyGLWidget::distanciaEntre2Punts(glm::vec3 orig, glm::vec3 dest)
{
    double x=dest.x-orig.x;
    double y=dest.y-orig.y;
    double z=dest.z-orig.z;
    double d = sqrt(x*x+y*y+z*z);
   // std::cout << "sqrt("<<x<<"²+"<<y<<"²+"<<z<<")="<<d<<std::endl;
    return d;
}

void MyGLWidget::distanciaMaximaEscena(){

    float dist1 = (maxCapsa.x-minCapsa.x)*2;
    float dist2 = (maxTerra.y-minTerra.y);
    float dist3 = (maxTerra.z-minTerra.z);


    radiEsc = dist1;
    if (radiEsc < dist2) radiEsc = dist2;
    if (radiEsc < dist3) radiEsc = dist3;

    radiEsc = radiEsc/2;
}

void MyGLWidget::refreshPosFocus() {
     glUniform3fv (posFocus, 1, &vec_posFocus[0]);
}

void MyGLWidget::refreshColFocus() {

    glUniform3fv (colFocus, 1, &vec_colFocus[0]);
}


//********************************************************//
//********************EVENTS******************************//


void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_O: { // canvia òptica entre perspectiva i axonomètrica
      perspectiva = !perspectiva;
      projectTransform ();
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
    xClick = e->x();
    yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }

  if (e->button() & Qt::RightButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ZOOM;
  }


}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
   //angleY += (e->x() - xClick) * M_PI / 2;

    angleX += (e->y() - yClick) * M_PI / 180.0;
    angleY += (e->x() - xClick) * M_PI / 180.0;

    viewTransform ();

  }

  //NOU_______________________________________
  if (DoingInteractive == ZOOM) {

      zoom += (e->y() - yClick)*0.001;
      projectTransform();

  }
  //NOU_________________________________________

  xClick = e->x();
  yClick = e->y();

  update ();
}


