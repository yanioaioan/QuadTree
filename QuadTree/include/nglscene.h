#ifndef NGLSCENE_H
#define NGLSCENE_H

#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/Colour.h>
#include <iostream>
#include <algorithm>
#include <ngl/Text.h>

//unsigned static int maxCapacity;

 struct Point
 {
     float x,y;

     Point(float _x,float _y):x(_x),y(_y){}
     bool operator==(const Point& a ) const {
             return a.x == x && a.y == y;
         }

 };

 typedef struct QuadTree
 {
     QuadTree(const QuadTree& t){x=t.x; y=t.y;}

     std::vector<Point>container;
     QuadTree *ul =NULL;
     QuadTree *ur =NULL;
     QuadTree *dl =NULL;
     QuadTree *dr =NULL;
     float x,y,width,height;
     QuadTree(float _x,float _y,float _width,float _height)
     {
         x =_x;
         y =_y;
         width =_width;
         height =_height;
     }


     unsigned int maxCapacity=100;

     void split()
     {
         float halfwidth =width /2;
         float halfheight =height /2;
         dl =new QuadTree(x,y,halfwidth,halfheight);
         dr =new QuadTree(x +halfwidth,y,halfwidth,halfheight);
         ul =new QuadTree(x,y +halfheight,halfwidth,halfheight);
         ur =new QuadTree(x+halfwidth,y+halfheight, halfwidth,halfheight);
         for(unsigned int i =0;i <container.size();i++)
         {
             ul->addPoint(container[i]);
             ur->addPoint(container[i]);
             dl->addPoint(container[i]);
             dr->addPoint(container[i]);
         }
     }


     void addPoint(Point &a)
     {
         //std::cout <<x <<std::endl;
         if(a.x >=x &&a.y >=y &&a.x <x +width &&a.y <y +height)
         {
             if(ul ==NULL)
             {
                 container.push_back(a);
                 if (container.size()>maxCapacity)
                     split();
             }
             else
             {
                 ul->addPoint(a);
                 ur->addPoint(a);
                 dl->addPoint(a);
                 dr->addPoint(a);
             }
         }
     }


//     void getPointCollisions(Point &a, QuadTree *tree)
//     {
//         //if tree node is a leaf node
//         if ( (tree->ul==NULL && tree->ur==NULL && tree->dl==NULL && tree->dr==NULL) /*&&  std::find(tree->container.begin(), tree->container.end(), a) !=tree->container.end()*/ )
//         {
//             //found element in root tree
//             std::vector<Point>::iterator element=std::find(tree->container.begin(), tree->container.end(), a);
//             if( element !=tree->container.end ())//found element
//             {
//                 //print out THE WHOLE container
////                  for(std::vector<Point>::iterator iter=tree->container.begin (); iter!=tree->container.end (); iter++)
////                  {
////                      std::cout<<"Point="<<(*iter).x<<","<<(*iter).y<<std::endl;
////                  }

//                std::cout<<"Search Point="<<(*element).x<<","<<(*element).y<<std::endl;
//                for(std::vector<Point>::iterator iter=tree->container.begin (); iter!=tree->container.end (); iter++)
//                {
//                    if(!((*iter)==a))
//                        std::cout<<"Collision Neighbour Point="<<(*iter).x<<","<<(*iter).y<<std::endl;
//                }


//                  return;
//             }



//         }
//         else//if it's a branch , find if one of the 4 sub-areas intersects with the searched area
//         {

//             if (a.x >= tree->dl->x && a.x <= tree->dl->x+tree->dl->width &&
//                 a.y >= tree->dl->y && a.y <= tree->dl->y+tree->dl->height)
//             {
//                 getPointCollisions (a,(tree->dl));
//             }

//             if (a.x >= tree->dr->x && a.x <= tree->dr->x+tree->dr->width &&
//                 a.y >= tree->dr->y && a.y <= tree->dr->y+tree->dr->height)
//             {
//                 getPointCollisions (a,(tree->dr));
//             }

//             if (a.x >= tree->ul->x && a.x <= tree->ul->x+tree->ul->width &&
//                 a.y >= tree->ul->y && a.y <= tree->ul->y+tree->ul->height)
//             {
//                 getPointCollisions (a,(tree->ul));
//             }

//             if (a.x >= tree->ur->x && a.x <= tree->ur->x+tree->ur->width &&
//                 a.y >= tree->ur->y && a.y <= tree->ur->y+tree->ur->height)
//             {
//                 getPointCollisions (a,(tree->ur));
//             }



////             // Decides in which quadrant the pixel lies,
////                 // and delegates the method to the appropriate node.

////             //Left areas
////                 if(a.x < tree->x + tree->width/ 2)
////                 {
////                   //Up Left
////                   if(a.y > tree->y + tree->height / 2)
////                   {
////                     getPointCollisions (a,(tree->ul));
////                   }
////                   else//Down Left
////                   {
////                     getPointCollisions (a,(tree->dl));
////                   }
////                 }
////                 else//Right areas
////                 {
////                     //Up Right
////                   if(a.y > tree->y + tree->height / 2)
////                   {
////                    getPointCollisions (a,(tree->ur));
////                   }
////                   else//Down Right
////                   {
////                     getPointCollisions (a,(tree->dr));
////                   }
////                 }






////             //ul
////             if (a.x >=tree.ul->x &&a.y >=tree.ul->y &&a.x <tree.ul->x +tree.ul->width /2 && a.y <tree.ul->y +tree.ul->height /2)
////             {
////                 getPointCollisions (a,*(tree.ul));
////             }
////             //ur
////             if (a.x >=tree.ur->x+tree.ur->width /2 &&a.y >=tree.ur->y &&a.x <tree.ur->x +tree.ur->width /2 && a.y <tree.ur->y +tree.ur->height /2)
////             {
////                 getPointCollisions (a,*(tree.ur));
////             }
////             //dl
////             if (a.x >=tree.dl->x &&a.y >=tree.dl->y+tree.dl->height /2 &&a.x <tree.dl->x +tree.dl->width /2 && a.y <tree.dl->y +tree.dl->height /2)
////             {
////                 getPointCollisions (a,*(tree.dl));
////             }
////             //dr
////             if (a.x >=tree.dr->x+tree.dr->width /2  &&a.y >=tree.dr->y+tree.dr->height /2 &&a.x <tree.dr->x +tree.dr->width /2 && a.y <tree.dr->y +tree.dr->height /2)
////             {
////                 getPointCollisions (a,*(tree.dr));
////             }



////             //found element in ul subtree
////             if( std::find(tree.ul->container.begin(), tree.ul->container.end(), a) !=tree.ul->container.end() )
////             {
////                 //print out container
////                  for(std::vector<Point>::iterator iter=tree.ul->container.begin (); iter!=tree.ul->container.end (); iter++)
////                  {
////                      std::cout<<(*iter).x<<","<<(*iter).y<<std::endl;
////                  }
////             }

////             //found element in ur subtree
////             if( std::find(tree.ur->container.begin(), tree.ur->container.end(), a) !=tree.ur->container.end() )
////             {
////                 //print out container
////                  for(std::vector<Point>::iterator iter=tree.ur->container.begin (); iter!=tree.ur->container.end (); iter++)
////                  {
////                      std::cout<<(*iter).x<<","<<(*iter).y<<std::endl;
////                  }
////             }

////             //found element in dl subtree
////             if( std::find(tree.dl->container.begin(), tree.dl->container.end(), a) !=tree.dl->container.end() )
////             {
////                 //print out container
////                  for(std::vector<Point>::iterator iter=tree.dl->container.begin (); iter!=tree.dl->container.end (); iter++)
////                  {
////                      std::cout<<(*iter).x<<","<<(*iter).y<<std::endl;
////                  }
////             }

////             //found element in dr subtree
////             if( std::find(tree.dr->container.begin(), tree.dr->container.end(), a) !=tree.dr->container.end() )
////             {
////                 //print out container
////                  for(std::vector<Point>::iterator iter=tree.dr->container.begin (); iter!=tree.dr->container.end (); iter++)
////                  {
////                      std::cout<<(*iter).x<<","<<(*iter).y<<std::endl;
////                  }
////             }



//         }

//     }

 }QuadTree;
 //Binary Search Tree


class NGLScene: public QGLWidget
{
    Q_OBJECT
public:

    NGLScene(QWidget *_parent);
    ~NGLScene();

    float m_rColor;
    std::vector<Point> treePositions;


protected:
    void initializeGL ();
    void resizeGL (QResizeEvent *_event);
    void loadMatricesToShader(ngl::Transformation &_transform, const ngl::Mat4 &_globalTx, ngl::Camera *_cam, ngl::Colour &c);
    void detectAndResolveCollisions(Point &a, std::vector<Point> *collisionAreaPoints, const float &width, const float &height);
    void getPointCollisions(Point &a, QuadTree *tree);

    void findTreeElements(QuadTree &tree);
    void deleteAreaByAreaElements(QuadTree &tree);

    void paintGL ();

    void timerEvent( QTimerEvent *_event );
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event);
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent * _event);
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent * _event );
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent(QWheelEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------

    void keyPressEvent(QKeyEvent *_event);

private:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the x rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the y rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the Right mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the global mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_mouseGlobalTX;
    int updateTimer;
    /// @brief flag to indicate if animation is active or not
    bool m_animate;
    ngl::Camera *m_cam;
    ngl::Transformation m_transform;
    ngl::Text *m_text;



signals:
    void clicked(bool);

public slots:
  void testButtonClicked(bool);


};

#endif // NGLSCENE_H
