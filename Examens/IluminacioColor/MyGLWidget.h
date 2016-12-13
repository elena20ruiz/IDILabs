#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event 
    // corresponent de ratolí
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);

  private:
    void createBuffers ();
    void carregaShaders ();
    void projectTransform ();
    void viewTransform ();

    void modelTransformTerra ();
    void modelTransformCaps1 ();
    void modelTransformCaps2 ();

    void calculaCapsaModel ();
    void calculaCapsaModel2 ();

    void refreshColFocus ();
    void refreshPosFocus ();

    //FUNCIONS IMPORTANTS-----------------------------

    void init_camera();
    float distanciaEntre2Punts(glm::vec3 orig, glm::vec3 dest);
    void distanciaMaximaEscena();


    // VAO i VBO names
    GLuint VAO_Caps, VBO_CapsPos, VBO_CapsNorm, VBO_CapsMatamb, VBO_CapsMatdiff, VBO_CapsMatspec, VBO_CapsMatshin;
    GLuint VAO_Caps2, VBO_Caps2Pos, VBO_Caps2Norm, VBO_Caps2Matamb, VBO_Caps2Matdiff, VBO_Caps2Matspec, VBO_Caps2Matshin;

    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraNorm, VBO_TerraMatamb, VBO_TerraMatdiff, VBO_TerraMatspec, VBO_TerraMatshin;
    // Program
    QOpenGLShaderProgram *program;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // attribute locations
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;
    //Iluminacio locations
    GLuint posFocus,colFocus;

    //SOBRE ESCENA:
    // model
    Model caps,caps2;
    //MAXIMS I MINIMS
    glm::vec3 maxCapsa,minCapsa,maxTerra,minTerra,maxCapsa2,minCapsa2;
    // paràmetres calculats a partir de la capsa contenidora del model
    glm::vec3 centreCaps,centreTerra,centreCaps2;
    float escala, escala2;
    // radi de l'escena
    float radiEsc;


    //SOBRE CAMARA
    float FOV,ra,znear,zfar;
    float zoom,rotate;
    glm::vec3 UP,OBS,VRP;

    //ROTACIONS
    float rx, ry;

    //ILUMINACIO
    glm::vec3 vec_posFocus, vec_colFocus;



    typedef  enum {NONE, ROTATE,ZOOM} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float angleY,angleX;
    bool perspectiva;
};

