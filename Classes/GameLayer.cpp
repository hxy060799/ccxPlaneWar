//
//  GameLayer.cpp
//  PlaneWar
//
//  Created by Bill on 13-8-24.
//
//

#include "GameLayer.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

//最后优化的时候，把Z在校对一次

//游戏暂停,playerPlane

//基本方法

GameLayer::~GameLayer()
{
    if(bullets!=NULL){
        bullets->release();
        bullets=NULL;
    }
    if(enemies!=NULL){
        enemies->release();
        enemies=NULL;
    }
    if(props!=NULL){
        props->release();
        props=NULL;
    }
    if(debugLabel!=NULL){
        debugLabel->release();
        debugLabel=NULL;
    }
    if(spriteFrameUsed!=NULL){
        spriteFrameUsed->release();
        spriteFrameUsed=NULL;
    }
    if(resolutionDisplay!=NULL){
        resolutionDisplay->release();
        resolutionDisplay=NULL;
    }
    if(backgroundSprite_1!=NULL){
        backgroundSprite_1->release();
        backgroundSprite_1=NULL;
    }
    if(backgroundSprite_2!=NULL){
        backgroundSprite_2->release();
        backgroundSprite_2=NULL;
    }
    SimpleAudioEngine::sharedEngine()->end();
}

CCScene* GameLayer::scene()
{
    
    CCScene* scene=CCScene::create();
    
    GameLayer* layer=GameLayer::create();
    
    scene->addChild(layer);
    
    return scene;
}

bool GameLayer::init()
{
    
    if(!CCLayer::init())
    {
        return false;
    }
    
    CCLog("HelloWorld");
    
    this->loadData();
    this->startGame();
    
    return true;
}

void GameLayer::loadData()
{
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,kCCMenuHandlerPriority,true);
    
    winSize=CCDirector::sharedDirector()->getWinSize();
    
    superBullet=false;
    imageScale=1;
    bombCount=0;
    score=0;
    
    bullets=CCArray::create();
    bullets->retain();
    enemies=CCArray::create();
    enemies->retain();
    props=CCArray::create();
    props->retain();
    
    this->loadSpriteFrames();
    this->loadBackgroundSprites();
    this->loadBombButton();
    this->loadScoreLabel();
    this->loadMusicAndSound();
    this->loadDebugLabel();
    this->loadGameEnd();
    this->loadPlayerPlane();
    this->loadPauseButton();
}

void GameLayer::startGame()
{
    gameStarted=true;
    gamePaused=false;
    
    this->showPlayerPlane(true);
    this->startShowProps();
    this->startBackgroundMoving();
    this->startShootBullet();
    this->startShowEnemies();
    this->startCheckCollision();
    this->playBackgroundMusic();
}

//暂停按钮
void GameLayer::loadPauseButton(){
    CCMenuItem*pauseButton=CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("game_pause.png"),
                                                    CCSprite::createWithSpriteFrameName("game_pause_hl.png")
                                                    ,this,menu_selector(GameLayer::pauseButtonPressed));
    pauseButton->setAnchorPoint(CCPoint(1,1));
    
    CCMenu* menu=CCMenu::create(pauseButton,NULL);
    menu->setAnchorPoint(CCPoint(1,1));
    menu->setPosition(CCPoint(winSize.width,winSize.height));
    this->addChild(menu);
}

void GameLayer::pauseButtonPressed(void* sender){
    if(gameStarted){
        if(!gamePaused){
            CCDirector::sharedDirector()->pause();
            CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
            gamePaused=true;
        }else{
            CCDirector::sharedDirector()->resume();
            CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
            gamePaused=false;
        }
    }
}

//调试信息

void GameLayer::loadDebugLabel()
{
    debugLabel=CCLabelTTF::create("Debug...", "Thonburi", 34);
    debugLabel->retain();
    debugLabel->setPosition(CCPoint(winSize.width/2, winSize.height/2));
    debugLabel->setColor(ccc3(0, 0, 0));
    this->addChild(debugLabel);
}

void GameLayer::toggleDebugLabel(){
    if(debugLabel->isVisible()==false){
        debugLabel->setVisible(true);
    }else{
        debugLabel->setVisible(false);
    }
}

//游戏结束
void GameLayer::loadGameEnd()
{
    restartLabel=CCLabelTTF::create("Restart", "Thonburi-Bolid", 60);
    restartLabel->setPosition(CCPoint(winSize.width/2, winSize.height*0.5));
    restartLabel->setColor(ccc3(255, 0, 0));
    restartLabel->setVisible(false);
    restartLabel->retain();
    this->addChild(restartLabel);
}

void GameLayer::showGameEnd()
{
    restartLabel->setVisible(true);
}
void GameLayer::hideGameEnd()
{
    restartLabel->setVisible(false);
}

//资源加载

void GameLayer::loadSpriteFrames()
{
    spriteFrameUsed=CCString::create("");
    
    if(winSize.width<=320){
        imageScale=winSize.width/320;
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameArts.plist");
        spriteFrameUsed=CCString::create("320x568");
    }else if(winSize.width<=640){
        imageScale=winSize.width/640;
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameArts-hd.plist");
        spriteFrameUsed=CCString::create("640x1136");
    }else if(winSize.width<=720){
        imageScale=winSize.width/720;
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameArts-android.plist");
        spriteFrameUsed=CCString::create("720x1280");
    }else{
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameArts-androidhd.plist");
        imageScale=winSize.width/1080;
        spriteFrameUsed=CCString::create("1080x1920");
        if(winSize.width>1080){
            spriteFrameUsed=CCString::create("Resolution Too High!");
        }
    }
    
    if(winSize.width/winSize.height<0.5625){
        resolutionDisplay=CCString::createWithFormat("%ix%i Screen Too Long",(int)winSize.width,(int)winSize.height);
    }else{
        resolutionDisplay=CCString::createWithFormat("%ix%i",(int)winSize.width,(int)winSize.height);
    }
    
    spriteFrameUsed->retain();
    resolutionDisplay->retain();

}

//背景控制

void GameLayer::loadBackgroundSprites()
{
    backgroundSprite_1=CCSprite::createWithSpriteFrameName("background_2.png");
    backgroundSprite_1->retain();
    backgroundSprite_2=CCSprite::createWithSpriteFrameName("background_2.png");
    backgroundSprite_2->retain();
    
    backgroundSprite_1->setScale(imageScale);
    backgroundSprite_2->setScale(imageScale);
    
    this->getBackgroundHeight();
    
    backgroundSprite_1->setAnchorPoint(CCPoint(0.5f,0));
    backgroundSprite_2->setAnchorPoint(CCPoint(0.5f,0));
    
    backgroundSprite_1->setPosition(CCPoint(winSize.width/2,0));
    backgroundSprite_2->setPosition(CCPoint(winSize.width/2,backgroundHeight));
    
    this->addChild(backgroundSprite_1,0);
    this->addChild(backgroundSprite_2,0);
    
}

void GameLayer::getBackgroundHeight()
{
    //减去2px可以让两个背景块有细微重叠,否则会看到两个背景块中间的细缝.
    backgroundHeight=backgroundSprite_1->boundingBox().size.height-2;
}

void GameLayer::startBackgroundMoving()
{
    if(backgroundSprite_1->numberOfRunningActions()==0){
        this->moveBackgroundDownWithSprite(backgroundSprite_1);
        this->moveBackgroundDownWithSprite(backgroundSprite_2);
    }else{
        CCActionManager*manager=CCDirector::sharedDirector()->getActionManager();
        manager->resumeTarget(backgroundSprite_1);
        manager->resumeTarget(backgroundSprite_2);
    }
}

void GameLayer::stopBackgroundMoving(bool pause)
{
    if(pause){
        CCActionManager*manager=CCDirector::sharedDirector()->getActionManager();
        manager->pauseTarget(backgroundSprite_1);
        manager->pauseTarget(backgroundSprite_2);
    }else{
        backgroundSprite_1->stopAllActions();
        backgroundSprite_2->stopAllActions();
    }
}

void GameLayer::moveBackgroundDownWithSprite(CCSprite* backgroundSprite)
{
    
    CCFiniteTimeAction* moveDown=(CCFiniteTimeAction*)CCMoveBy::create(5.0f, CCPoint(0,-backgroundHeight));
    
    CCFiniteTimeAction* moveEnd=(CCFiniteTimeAction*)CCCallFuncND::create(this, callfuncND_selector(GameLayer::spriteMoveEndedWithAction),backgroundSprite);
    
    backgroundSprite->runAction(CCSequence::create(moveDown,moveEnd,NULL));
    
}

void GameLayer::spriteMoveEndedWithAction(CCAction* action,CCSprite* backgroundSprite)
{
    if(backgroundSprite->getPosition().y==-backgroundHeight){
        backgroundSprite->setPosition(CCPoint(winSize.width/2,backgroundHeight));
    }
    this->moveBackgroundDownWithSprite(backgroundSprite);
}

//玩家飞机加载

void GameLayer::loadPlayerPlane()
{
    playerPlane=CCSprite::createWithSpriteFrameName("hero_fly_1.png");
    playerPlane->setScale(imageScale);
    //playerPlane->setPosition(CCPoint(winSize.width/2,0.1*winSize.height));
    this->addChild(playerPlane,3);
    CCAction* playerAction=this->animationWithSpriteFrames("hero_fly_%i.png", 2, 0.2f, 0);
    playerPlane->runAction(playerAction);
}

void GameLayer::showPlayerPlane(bool resetPosition)
{
    if(resetPosition){
        //隐藏的时候动画会被释放掉，这里重新生成
        CCAction* playerAction=this->animationWithSpriteFrames("hero_fly_%i.png", 2, 0.2f, 0);
        playerPlane->runAction(playerAction);
        
        playerPlane->setPosition(CCPoint(winSize.width/2,0.1*winSize.height));
    }
    playerPlane->setVisible(true);
}

void GameLayer::hidePlayerPlane()
{
    playerPlane->setVisible(false);
}


//子弹的生成与控制

void GameLayer::startShootBullet()
{
    this->schedule(schedule_selector(GameLayer::shootBullet),0.2f);
}

void GameLayer::stopShootBullet()
{
    this->unschedule(schedule_selector(GameLayer::shootBullet));
}

void GameLayer::shootBullet()
{
    CCSprite* bullet=CCSprite::createWithSpriteFrameName((!superBullet)?"bullet1.png":"bullet2.png");
    bullet->setScale(imageScale);
    bullet->setAnchorPoint(CCPoint(0.5,0));
    bullet->setPosition(CCPoint(playerPlane->getPosition().x,playerPlane->getPosition().y+playerPlane->boundingBox().size.height/2));
    this->addChild(bullet,2);
    
    bullets->addObject(bullet);
    
    //加上2px可以防止子弹在移除视图之前消失的错觉影响美观.
    CCFiniteTimeAction* bulletMoveUp=CCMoveBy::create(0.5f, CCPoint(0,winSize.height-playerPlane->getPosition().y+2));
    CCFiniteTimeAction* bulletMoveEnd=CCCallFuncND::create(this, callfuncND_selector(GameLayer::bulletMoveEnded), bullet);
    
    bullet->runAction(CCSequence::create(bulletMoveUp,bulletMoveEnd,NULL));
    
    SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("bullet.mp3").c_str());
}

void GameLayer::bulletMoveEnded(CCAction* action,CCSprite* bulletSprite)
{
    bulletSprite->removeFromParentAndCleanup(true);
    bullets->removeObject(bulletSprite);
}

//敌机的生成和控制

void GameLayer::startShowEnemies()
{
    this->schedule(schedule_selector(GameLayer::showEnemy),0.2);
}

void GameLayer::stopShowEnemies()
{
    this->unschedule(schedule_selector(GameLayer::showEnemy));
}

void GameLayer::showEnemy()
{
    
    //屏幕最大飞机数
    int maxPlane=(int)((score<1000000)?(0.000014*score+2+0.5):16);
    int speedPercent=(int)((score<1000000)?((0.000004*score+1)*100):100);
    
    //maxPlaneLabel->setString(CCString::createWithFormat("Max:%i",maxPlane)->getCString());
    //speedLabel->setString(CCString::createWithFormat("Speed:%i",speedPercent)->getCString());
    
    int planeCount=enemies->count();
    
    if(planeCount>=maxPlane){
        return;
    }
    
    float speedToUse=this->randomNumber(speedPercent-10, speedPercent+10);
    speedToUse/=100;
    
    
    float smallPlane=(score<1000000)?(-0.0000005*score+1):0.5f;
    float mediumPlane=(score<1000000)?(0.0000003*score):0.3f;
    
    float planeType=(float)(this->randomNumber(0, 10000))/10000;
    
    CCString* debugStirng=CCString::createWithFormat("R=%s\nS=%s\n%f\nMax=%i\nSpeed=%i",resolutionDisplay->getCString(),spriteFrameUsed->getCString(),imageScale,maxPlane,speedPercent);
    debugLabel->setString(debugStirng->getCString());
    
    int type=0;
    int hp=0;
    if(planeType<=smallPlane){
        type=1;
        hp=1;
    }else if(planeType<=smallPlane+mediumPlane){
        type=3;
        hp=5;
    }else{
        //屏幕上不会同时出现多与三个大飞机
        int sum=0;
        for(int i=0;i<enemies->count();i++){
            if(this->getEnemyTypeByTag(((CCSprite*)enemies->objectAtIndex(i))->getTag())==2){
                sum++;
            }
        }
        if(sum<3){
            type=2;
            hp=10;
        }else{
            type=3;
            hp=5;
        }
    }
    
    CCSprite* enemy=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("enemy%i_fly_1.png",type)->getCString());
    enemy->setScale(imageScale);
    enemy->setAnchorPoint(CCPoint(0.5,0));
    //替换这里
    int min=(int)playerPlane->boundingBox().size.width/2;
    int max=(int)winSize.width-playerPlane->boundingBox().size.width/2;
    enemy->setPosition(CCPoint(this->randomNumber(min,max),winSize.height));
    this->addChild(enemy,4);
    
    //Tag用于记录敌机类型和HP值(这比再写一个类要方便多了)
    enemy->setTag(type*100+hp);
    
    enemies->addObject(enemy);
    
    CCFiniteTimeAction* enemyMoveDown=CCMoveBy::create(8.0f/speedToUse, CCPoint(0,-winSize.height-enemy->boundingBox().size.height));
    CCFiniteTimeAction* enemyMoveEnd=CCCallFuncND::create(this, callfuncND_selector(GameLayer::enemyMoveEnded),enemy);
    
    enemy->runAction(CCSequence::create(enemyMoveDown,enemyMoveEnd,NULL));
}

void GameLayer::enemyMoveEnded(CCAction* action,CCSprite* enemySprite)
{
    enemySprite->removeFromParentAndCleanup(true);
    enemies->removeObject(enemySprite);
}

//道具的生成和控制

void GameLayer::startShowProps()
{
    this->schedule(schedule_selector(GameLayer::showProp),40.0f);
}

void GameLayer::stopShowProps()
{
    this->unschedule(schedule_selector(GameLayer::showProp));
}

void GameLayer::showProp()
{
    
    if(arc4random()*2==1){
        return;
    }
    
    int type=arc4random()%2+4;
    
    CCSprite* prop=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("enemy%i_fly_1.png",type)->getCString());
    prop->setScale(imageScale);
    prop->setAnchorPoint(CCPoint(0.5,0));
    prop->setPosition(CCPoint(arc4random()%(int)(winSize.width+1),winSize.height));
    this->addChild(prop,4);
    
    prop->setTag(type*100);
    props->addObject(prop);
    
    CCFiniteTimeAction* propMoveDown1=CCMoveBy::create(0.2f,CCPoint(0,-(this->randomNumber((winSize.height*0.2),(winSize.height*0.4)))));
    CCFiniteTimeAction* propMoveUp=CCMoveTo::create(0.5f,CCPoint(prop->getPosition().x,winSize.height));
    CCFiniteTimeAction* propMoveDown2=CCMoveBy::create(1.0f, CCPoint(0,-winSize.height-prop->boundingBox().size.height));
    CCFiniteTimeAction* propMoveEnd=CCCallFuncND::create(this,callfuncND_selector(GameLayer::propsMoveEnded),prop);
    
    prop->runAction(CCSequence::create(propMoveDown1,propMoveUp,propMoveDown2,propMoveEnd,NULL));
    
    SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy4_out.mp3").c_str());
}

void GameLayer::propsMoveEnded(CCAction* action,CCSprite* propSprite)
{
    propSprite->removeFromParentAndCleanup(true);
    props->removeObject(propSprite);
}

void GameLayer::cancelSuperBullet()
{
    superBullet=false;
}

void GameLayer::loadBombButton()
{
    bomb=CCSprite::createWithSpriteFrameName("bomb.png");
    bomb->setScale(imageScale);
    bomb->setAnchorPoint(CCPoint(0,0));
    bomb->setPosition(CCPoint(winSize.width*0.05,winSize.width*0.05));
    this->addChild(bomb);
    bomb->setVisible(false);
}

//分数标签

void GameLayer::loadScoreLabel()
{
    scoreLabel=CCLabelTTF::create("Score:0","MarkerFelt-Thin",winSize.width*0.0625);
    scoreLabel->setColor(ccc3(0, 0, 0));
    scoreLabel->setAnchorPoint(CCPoint(0,1));
    scoreLabel->setPosition(CCPoint(0,winSize.height));
    this->addChild(scoreLabel);
}

//碰撞检测

void GameLayer::startCheckCollision()
{
    this->schedule(schedule_selector(GameLayer::checkCollision));
}

void GameLayer::stopCheckCollision()
{
    this->unschedule(schedule_selector(GameLayer::checkCollision));
}

void GameLayer::checkCollision()
{
    CCArray* readyToRemoveEnemies=CCArray::create();
    CCArray* readyToRemoveBullets=CCArray::create();
    
    for(int i=0;i<enemies->count();i++){
        CCSprite* enemy=(CCSprite*)enemies->objectAtIndex(i);
        if(this->CCRectIntersect(enemy->boundingBox(),this->makeNewRect(CCSize(0.3*playerPlane->boundingBox().size.width,0.8*playerPlane->boundingBox().size.height),ccpSub(playerPlane->getPosition(),CCPoint(0,playerPlane->boundingBox().size.height/2)),CCPoint(0.5,0)))){
            
            SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("game_over.mp3").c_str());
            
            //精简这里
            this->stopCheckCollision();
            this->stopShootBullet();
            this->stopBackgroundMoving(true);
            this->stopShowEnemies();
            this->stopShowProps();
            SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
            
            gameStarted=false;
            
            CCFiniteTimeAction* gameOverAction=this->animationWithSpriteFrames("hero_blowup_%i.png", 4, 0.1f, 1);
            CCFiniteTimeAction* actionEnd=CCCallFuncND::create(this, callfuncND_selector(GameLayer::gameOverBlowUpEnded),playerPlane);
            playerPlane->stopAllActions();
            playerPlane->runAction(CCSequence::create(gameOverAction,actionEnd,NULL));
        }
        for(int j=0;j<bullets->count();j++){
            CCSprite* bullet=(CCSprite*)bullets->objectAtIndex(j);
            if(this->CCRectIntersect(enemy->boundingBox(), bullet->boundingBox())){
                if(enemy->boundingBox().origin.y+enemy->boundingBox().size.height<winSize.height){
                    readyToRemoveBullets->addObject(bullet);
                    
                    if(this->getEnemyHpByTag(enemy->getTag())>=2&&superBullet){
                        enemy->setTag(enemy->getTag()-2);
                    }else{
                        enemy->setTag(enemy->getTag()-1);
                    }
                    int hp=this->getEnemyHpByTag(enemy->getTag());
                    if(hp<=0){
                        readyToRemoveEnemies->addObject(enemy);
                    }else{
                        int type=this->getEnemyTypeByTag(enemy->getTag());
                        if(type==2){
                            CCFiniteTimeAction* hitAction=this->animationWithSpriteFrames("enemy2_hit_%i.png",2,0.1f,1);
                            enemy->runAction(hitAction);
                        }else if(type==3){
                            CCFiniteTimeAction* hitAction=this->animationWithSpriteFrames("enemy3_hit_%i.png",2,0.1f,1);
                            enemy->runAction(hitAction);
                        }
                    }
                }
            }
        }
    }
    
    for(int i=0;i<readyToRemoveBullets->count();i++){
        CCSprite* bullet=(CCSprite*)readyToRemoveBullets->objectAtIndex(i);
        
        bullets->removeObject(bullet);
        bullet->removeFromParentAndCleanup(true);
    }
    
    for(int i=0;i<readyToRemoveEnemies->count();i++){
        CCSprite* enemy=(CCSprite*)readyToRemoveEnemies->objectAtIndex(i);
        
        enemies->removeObject(enemy);
        this->blowUpWithEnemy(enemy);
    }
    
    CCArray* readyToRemoveProps=CCArray::create();
    
    for(int i=0;i<props->count();i++){
        CCSprite* prop=(CCSprite*)props->objectAtIndex(i);
        if(CCRectIntersect(prop->boundingBox(),playerPlane->boundingBox())){
            readyToRemoveProps->addObject(prop);
            int type=this->getEnemyTypeByTag(prop->getTag());
            if(type==5){
                this->unschedule(schedule_selector(GameLayer::cancelSuperBullet));
                if(!superBullet){
                    superBullet=true;
                    this->schedule(schedule_selector(GameLayer::cancelSuperBullet),20.0f,1,0.0f);
                }
                SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy5_down.mp3").c_str());
            }else if(type==4){
                bombCount++;
                bomb->setVisible(true);
                SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy4_down.mp3").c_str());
            }
        }
    }
    
    for(int i=0;i<readyToRemoveProps->count();i++){
        CCSprite* prop=(CCSprite*)readyToRemoveProps->objectAtIndex(i);
        props->removeObject(prop);
        prop->removeFromParentAndCleanup(true);
    }
    
}

//飞机爆炸

void GameLayer::blowUpWithEnemy(CCSprite* enemy)
{
    
    int type=this->getEnemyTypeByTag(enemy->getTag());
    
    CCFiniteTimeAction* blowUpAction;
    
    switch(type){
        case 1:
            score+=1000;
            blowUpAction=this->animationWithSpriteFrames("enemy1_blowup_%i.png",4,0.1f,1);
            SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy1_down.mp3").c_str());
            break;
        case 2:
            score+=30000;
            blowUpAction=this->animationWithSpriteFrames("enemy2_blowup_%i.png",7,0.1f,1);
            SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy2_down.mp3").c_str());
            break;
        case 3:
            score+=10000;
            blowUpAction=this->animationWithSpriteFrames("enemy3_blowup_%i.png",4,0.1f,1);
            SimpleAudioEngine::sharedEngine()->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy3_down.mp3").c_str());
            break;
    }
    
    scoreLabel->setString(CCString::createWithFormat("Score:%i",score)->getCString());
    
    CCFiniteTimeAction* enemyBlowUpEnd=CCCallFuncND::create(this, callfuncND_selector(GameLayer::enemyBlowUpEnded),enemy);
    enemy->stopAllActions();
    enemy->runAction(CCSequence::create(blowUpAction,enemyBlowUpEnd,NULL));
}

void GameLayer::enemyBlowUpEnded(CCAction* action,CCSprite* enemySprite)
{
    enemySprite->removeFromParentAndCleanup(true);
}

void GameLayer::gameOverBlowUpEnded(CCAction* action,CCSprite* planeSprite)
{
    this->hidePlayerPlane();
    score=0;
    this->showGameEnd();
}

//动画管理

CCFiniteTimeAction* GameLayer::animationWithSpriteFrames(const char* frameName,int frameCount,float delayTime,int repeatTimes)
{
    CCArray* animationFrames=CCArray::create();
    for(int i=0;i<frameCount;i++){
        CCString* frameNameCurrent=CCString::createWithFormat(frameName,i+1);
        
        animationFrames->addObject((CCAnimationFrame*)CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameNameCurrent->getCString()));
    }
    
    CCAnimation* animation=CCAnimation::createWithSpriteFrames(animationFrames, delayTime);
    
    CCFiniteTimeAction* action;
    //RepeatTimes<=0为永久循环
    if(repeatTimes<=0){
        action=CCRepeatForever::create(CCAnimate::create(animation));
    }else{
        action=CCRepeat::create(CCAnimate::create(animation), repeatTimes);
    }
    
    return action;
}

//触摸处理

bool GameLayer::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint touchLocation=pTouch->getLocationInView();
    touchLocation=CCDirector::sharedDirector()->convertToGL(touchLocation);
    
    if(gameStarted&&!gamePaused){
        if(CCRectContainsPoint(touchLocation,bomb->boundingBox())){
            if(bombCount>0&&enemies->count()>0){
                bombCount--;
                if(bombCount==0){
                    bomb->setVisible(false);
                }
                
                for(int i=0;i<enemies->count();i++){
                    CCSprite* enemy=(CCSprite*)enemies->objectAtIndex(i);
                    this->blowUpWithEnemy(enemy);
                }
                
                enemies->removeAllObjects();
            }
        }
    }else{
        if(!gameStarted){
            if(CCRectContainsPoint(touchLocation,restartLabel->boundingBox())){
                this->hideGameEnd();
                this->startGame();
            }
        }
    }
    return true;
}

void GameLayer::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if(gameStarted&&!gamePaused){
        CCPoint touchLocation=pTouch->getLocationInView();
        touchLocation=CCDirector::sharedDirector()->convertToGL(touchLocation);
        
        CCPoint oldTouchLocation=pTouch->getPreviousLocationInView();
        oldTouchLocation=CCDirector::sharedDirector()->convertToGL(oldTouchLocation);
        
        CCPoint translation=ccpSub(touchLocation,oldTouchLocation);
        
        if(CCRectContainsPoint(touchLocation,playerPlane->boundingBox())){
            CCPoint newPos = ccpAdd(playerPlane->getPosition(),translation);
            if(this->CCRectContainsRect(this->makeNewRect(playerPlane->boundingBox().size, newPos, ccp(0.5,0.5)),CCRect(0,0,winSize.width,winSize.height))){
                playerPlane->setPosition(newPos);
            }
        }
    }
}

void GameLayer::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    
    
}

void GameLayer::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent)
{
    
}

//按键处理

void GameLayer::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void GameLayer::keyMenuClicked()
{
    toggleDebugLabel();
}

//音效处理

void GameLayer::loadMusicAndSound()
{
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(CCFileUtils::sharedFileUtils()->fullPathForFilename("game_music.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy1_down.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy2_down.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy3_down.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy4_down.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy5_down.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("enemy4_out.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("game_over.mp3").c_str());
    SimpleAudioEngine::sharedEngine()->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("bullet.mp3").c_str());
}

void GameLayer::playBackgroundMusic()
{
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic(CCFileUtils::sharedFileUtils()->fullPathForFilename("game_music.mp3").c_str(),true);
}

//信息函数

int GameLayer::getEnemyTypeByTag(unsigned int tag)
{
    return (tag-tag%100)/100;
}

int GameLayer::getEnemyHpByTag(unsigned int tag)
{
    return tag%100;
}

//数学函数

int GameLayer::randomNumber(int min,int max)
{
    //整数随机数
    //min - x-1+min
    int value = (arc4random() % (max+1-min))+min;
    return value;
}

//几何算法

CCRect GameLayer::makeNewRect(CCSize size,CCPoint point,CCPoint anchorPoint)
{
    return CCRect(point.x-anchorPoint.x*size.width,point.y-anchorPoint.y*size.width,size.width,size.height);
}

bool GameLayer::CCRectContainsPoint(const CCPoint& point,const CCRect& rect)
{
    bool bRet=false;
    if(point.x>=rect.origin.x && point.x<(rect.size.width+rect.origin.x)
       && point.y>=rect.origin.y && point.y<(rect.size.height+rect.origin.y)){
        bRet = true;
    }
    
    return bRet;
}

bool GameLayer::CCRectContainsRect(const CCRect& rectA,const CCRect& rectB)
{
    //A在B里面
    bool bRet=false;
    if(rectA.origin.x>=rectB.origin.x && (rectA.size.width+rectA.origin.x)<(rectB.size.width+rectB.origin.x)
       && rectA.origin.y>=rectB.origin.y && (rectA.size.height+rectA.origin.y)<(rectB.size.height+rectB.origin.y)){
        bRet = true;
    }
    
    return bRet;
}

bool GameLayer::CCRectIntersect(const CCRect rc1, const CCRect& rc2)
{
    return  (  (std::max(rc1.origin.x, rc1.origin.x+rc1.size.width) >= std::min(rc2.origin.x, rc2.origin.x+rc2.size.width))
             && (std::max(rc2.origin.x, rc2.origin.x+rc2.size.width) >= std::min(rc1.origin.x, rc1.origin.x+rc1.size.width))
             && (std::max(rc1.origin.y, rc1.origin.y+rc1.size.height) >= std::min(rc2.origin.y, rc2.origin.y+rc2.size.height))
             && (std::max(rc2.origin.y, rc2.origin.y+rc2.size.height) >= std::min(rc1.origin.y, rc1.origin.y+rc1.size.height))
             );
}
