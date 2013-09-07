//
//  GameLayer.h
//  PlaneWar
//
//  Created by Bill on 13-8-24.
//
//

#ifndef __PlaneWar__GameLayer__
#define __PlaneWar__GameLayer__

#include "cocos2d.h"

class GameLayer:public cocos2d::CCLayer
{
public:
    ~GameLayer();
    static cocos2d::CCScene* scene();
    virtual bool init();
    CREATE_FUNC(GameLayer);
    bool gamePaused;
    bool gameStarted;
private:
    
    //基本方法
    cocos2d::CCSize winSize;
    void loadData();
    void startGame();
    
    //暂停按钮
    cocos2d::CCSprite* pauseButton;
    void loadPauseButton();
    void pauseButtonPressed(void* sender);
    
    //调试信息
    cocos2d::CCLabelTTF* debugLabel;
    cocos2d::CCString* spriteFrameUsed;
    cocos2d::CCString* resolutionDisplay;
    void loadDebugLabel();
    void toggleDebugLabel();
    
    //游戏结束
    cocos2d::CCLabelTTF* scoreLabal;
    cocos2d::CCLabelTTF* restartLabel;
    void loadGameEnd();
    void showGameEnd();
    void hideGameEnd();
    
    //资源加载
    float imageScale;
    void loadSpriteFrames();
    
    //背景控制
    float backgroundHeight;
    cocos2d::CCSprite* backgroundSprite_1;
    cocos2d::CCSprite* backgroundSprite_2;
    void loadBackgroundSprites();
    void getBackgroundHeight();
    void startBackgroundMoving();
    void stopBackgroundMoving(bool pause);
    void moveBackgroundDownWithSprite(cocos2d::CCSprite* backgroundSprite);
    void spriteMoveEndedWithAction(cocos2d::CCAction* action,cocos2d::CCSprite* backgroundSprite);

    //玩家飞机加载
    cocos2d::CCSprite* playerPlane;
    void loadPlayerPlane();
    void showPlayerPlane(bool resetPosition);
    void hidePlayerPlane();
    
    //子弹的生成和控制
    bool superBullet;
    cocos2d::CCArray* bullets;
    void startShootBullet();
    void stopShootBullet();
    void shootBullet();
    void bulletMoveEnded(cocos2d::CCAction* action,cocos2d::CCSprite* bulletSprite);
    
    //敌机的生成和控制
    cocos2d::CCArray* enemies;
    void startShowEnemies();
    void stopShowEnemies();
    void showEnemy();
    void enemyMoveEnded(cocos2d::CCAction* action,cocos2d::CCSprite* enemySprite);
    
    //道具的生成和控制
    int bombCount;
    cocos2d::CCArray* props;
    cocos2d::CCSprite* bomb;
    void startShowProps();
    void stopShowProps();
    void showProp();
    void propsMoveEnded(cocos2d::CCAction* action,cocos2d::CCSprite* propSprite);
    void cancelSuperBullet();
    void loadBombButton();
    
    //分数标签
    int score;
    cocos2d::CCLabelTTF* scoreLabel;
    void loadScoreLabel();
    
    //碰撞检测
    void startCheckCollision();
    void stopCheckCollision();
    void checkCollision();
    void blowUpWithEnemy(cocos2d::CCSprite* enemy);
    void enemyBlowUpEnded(cocos2d::CCAction* action,cocos2d::CCSprite* enemySprite);
    void gameOverBlowUpEnded(cocos2d::CCAction* action,cocos2d::CCSprite* planeSprite);
    
    //动画管理
    cocos2d::CCFiniteTimeAction* animationWithSpriteFrames(const char* frameName,int frameCount,float delayTime,int repeatTimes);
    
    //触摸处理
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    //按键处理
    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
    //音效处理
    void loadMusicAndSound();
    void playBackgroundMusic();
    
    //信息函数
    int getEnemyTypeByTag(unsigned int tag);
    int getEnemyHpByTag(unsigned int tag);
    
    //数学函数
    int randomNumber(int min,int max);
    
    //几何算法
    cocos2d::CCRect makeNewRect(cocos2d::CCSize size,cocos2d::CCPoint point,cocos2d::CCPoint anchorPoint);
    bool CCRectContainsPoint(const cocos2d::CCPoint& point,const cocos2d::CCRect& rect);
    bool CCRectContainsRect(const cocos2d::CCRect& rectA,const cocos2d::CCRect& rectB);
    bool CCRectIntersect(const cocos2d::CCRect rc1, const cocos2d::CCRect& rc2);
    
};
#endif 
