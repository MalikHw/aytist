#include <Geode/Geode.hpp>
#include <user95401.gif-sprites/include/CCGIFAnimatedSprite.hpp>

using namespace geode::prelude;

class AytistSprite : public CCNode {
public:
    CCGIFAnimatedSprite* m_gif = nullptr;
    int m_effectID = -1;

    static AytistSprite* create(CCNode* parent) {
        auto ret = new AytistSprite();
        if (ret && ret->init(parent)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(CCNode* parent) {
        if (!CCNode::init()) return false;

        m_gif = CCGIFAnimatedSprite::create("aytist.gif"_spr);
        if (!m_gif) return false;

        m_gif->setLoop(false);
        this->addChild(m_gif);

        auto size = parent->getContentSize();
        float x = (float)(rand() % (int)size.width);
        float y = (float)(rand() % (int)size.height);
        this->setPosition(ccp(x, y));
        this->setContentSize(CCSizeMake(144.f, 144.f));
        m_gif->setContentSize(CCSizeMake(144.f, 144.f));
        m_gif->setPosition(ccp(72.f, 72.f));

        return true;
    }

    void play() {
        m_gif->play();

        m_effectID = FMODAudioEngine::get()->playEffect("aytist.mp3"_spr);

        this->schedule(schedule_selector(AytistSprite::checkDone), 0.05f);
    }

    void checkDone(float dt) {
        if (!m_gif->isPlaying()) {
            this->unschedule(schedule_selector(AytistSprite::checkDone));

            if (m_effectID != -1) {
                FMODAudioEngine::get()->stopChannel(m_effectID);
                m_effectID = -1;
            }

            this->removeFromParentAndCleanup(true);
        }
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool m_enabled = true;
        CCScheduler* m_scheduler = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        m_fields->m_enabled = Mod::get()->getSettingValue<bool>("enabled");

        if (m_fields->m_enabled) {
            this->scheduleSpawns();
        }

        return true;
    }

    void scheduleSpawns() {

        float delay = 1.0f + (float)(rand() % 400) / 100.0f;
        this->scheduleOnce(schedule_selector(MyPlayLayer::spawnAytist), delay);
    }

    void spawnAytist(float) {
        if (!Mod::get()->getSettingValue<bool>("enabled")) {

            scheduleSpawns();
            return;
        }

        auto sprite = AytistSprite::create(this);
        if (sprite) {

            this->addChild(sprite, 100);
            sprite->play();
        }

        scheduleSpawns();
    }

    void onQuit() {
        this->unscheduleAllSelectors();
        PlayLayer::onQuit();
    }
};
