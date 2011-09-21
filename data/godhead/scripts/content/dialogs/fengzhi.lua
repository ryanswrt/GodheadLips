-- Link to LoS dialog format guide: http://sourceforge.net/apps/trac/lipsofsuna/wiki/Scripting/Dialogs
-- Link to Godhead's code style guidelines: http://dungeonhack.sourceforge.net/Code_Style_Guidelines

Dialogspec{name = "fengzhi", unique = true, {"branch", cond_dead = true, {"loot"}},

{"choice", "Hello!",
{"say", "Fengzhi", "Hello, stranger."},
},
{"choice", "Good day to you, friend.", check = "!flag:FengMet",
{"say", "Fengzhi", "That's quite an assumption you make there, stranger."}, {"flag", "FengMet"},
    {"choice", "Consider it a wishful thought, then, stranger."},
    {"say", "Fengzhi", "You start with presumptions, and follow up with imperatives. The illusion of control must be very dear to you."},
       {"choice", "Perhaps so. I am <avatar_name>. What may I call you then?"},
       {"say", "Fengzhi", "What would you call me? I don't think of myself as my name, but I appreciate that your existence must be quite simple, <avatar_name>."},
           {"choice", "It is. I'm simply lost! Perhaps I'll stay lost awhile... Stranger. You're a strange sort, but in a likable way. Are you lost too?",
           {"say", "Fengzhi", "You've asked too many questions, <avatar_gender>."},
           {"say", "Fengzhi", "Let me instead ask you, why do you presume that asking me anything will lead to anything? Am I not already where you are? Where else would I bring you?"},
           {"say", "Fengzhi", "You ask if I am lost before you know if I have direction. Riddle me this, if I did not know where I was going, could I ever be lost?"}, {"exit"}
           }
},
{"choice", "Oh. Hello, sir! I didn't expect to find anyone out here.", check = "!flag:FengMet",
{"say", "Fengzhi", "Oh I see. Straight into it are we? Well, I don't believe you've introduced yourself."}, {"flag", "FengMet"},
    {"choice", "I am named <avatar_name>."},
    {"say", "Fengzhi", "Greetings, wanderer. Might I assume you're lost?"},
       {"choice", "Well, um.. stranded I guess. Not entirely sure how I arrived. Everything is ...hazy."},
       {"say", "Fengzhi", "You guess? What do you know, then?"},
           {"choice", "You're a philosopher?"},
           {"say", "Fengzhi", "Do you know that I am?"},
               {"choice", "...Yes, quite."},
               {"say", "Fengzhi", "So here is what I know. You are what you know, moved by what you don't know."},
               {"say", "Fengzhi", "Once this has been realized there can be no doubt as to identity or direction, and we can never be lost."},
               {"say", "Fengzhi", "We are always what we know that we are, and we know that our destination is unknown. You are not lost."},
                   {"choice", "Are you sure? I think you lost me.",
                   {"say", "Fengzhi", "The moment you know where you are headed that destination becomes part of your identity. You are <avatar_name> heading somewhere."},
                       {"choice","Then, I'm <avatar_name> headed for... for.. Hyalang was it?"},
                       {"say", "Fengzhi", "If you believe so..."}
                   },
                   {"choice", "Ah. So... [thoughtful gesture]",
                   {"say", "Fengzhi", "Where are you headed, then, stranger?"},
                       {"choice","I was... trying to reach... Hyalang... [head feels horrible, like a concussion]"}
                   },
                       {"say", "Fengzhi", "Pleased to meet you, <avatar_name>, traveling to Hyalang. What is your purpose?"},
                           {"choice", "Ow, my head... Yes, I just wanted to get away from the violence at home.",
                           {"say", "Fengzhi","Have you succeeded?"},
                               {"choice", "It doesn't seem like it. But is there fighting here too?",
                                   {"say", "Fengzhi","The world is constantly at war with itself and death's triumph over life is the ultimate key to life itself."},
                                   {"say", "Fengzhi","Without death, there could never be life. I know this.."}
                               }
                           },
                           {"choice", "What is yours?",
                           {"say", "Fengzhi","My purpose is to defy purpose, and find meaning in that which never meant anything."},
                               {"choice", "Odd, then, you would seem to assume *I* am concerned with a purpose.",
                               {"say", "Fengzhi","You told me you were lost, implying a desire for direction. We have sorted that out. You are not lost, and this conversation serves no purpose. Be on your way."}, {"exit"}
                               },
                               {"choice","How is that going?",
                               {"say", "Fengzhi"," Not well, I'm afraid. I'm rather lost, you see."},
                                   {"choice","Does that mean you doubt your identity or your direction?",
                                   {"say", "Fengzhi","There is no doubt. I know that I am unaware. I am void, a vessel for knowledge that is yet to be revealed. As such, my identity is my direction."},
                                       {"choice","Then how are you lost? Do you not know the way to any town or village here?",
                                       {"say", "Fengzhi"," Follow the coast to the right and you will be in Port Hyalang. I am not lost in space, stranger, but in essence."}
                                       },
                                   },
                                   {"choice", "Did you chose to come to these old ruins to become unlost, or are you even concerned with being something other than lost?",
                                   {"say", "Fengzhi"," I chose to be here, right now. I am a vessel for knowledge that is yet to be revealed. I am the memory of countless deaths before me."},
                                   {"say", "Fengzhi","I am alive here, right now."}
                                   },
                                   {"choice"," Great. Well, that makes two of us. Do you know what place this is?",
                                   {"say", "Fengzhi", "This is the ruined port of Hyalang. Great men lived here once. Their deaths left this place empty, ready to be seeded with new life. Death will claim them, too, and life's triumph will be absolute."}
                                   }
                               },
                               {"choice", "You'd like my uncle, then.",
                               {"say", "Fengzhi"," I salute his life and celebrate his inevitable death."},
                               --This sets a flag, that can be checked in dialogs in order to see if topics are known
                                   {"choice","Why do you celebrate death?", check ="!flag:CelebrateDeath",
                                   {"say", "Fengzhi"," If there was no death, there could be no life. Death's inevitability is the very force behind life. Death is the Creator. We must celebrate it."}
                                   },
                                   {"choice","Are we having a party?!",
                                   {"say", "Fengzhi"," Seems to me the party is already over. You don't look so good."}, {"exit"}
                                   }
                               }
                           }
},
{"choice", "What is this place?",
{"say", "Fengzhi", "This is the ruined port of Hyalang. Great men lived here once."},
{"say", "Fengzhi", "Their deaths left this place empty, ready to be seeded with new life. Death will claim them, too, and life's triumph will be absolute."},
    {"choice", "What was so great about them?"},
    {"say", "Fengzhi", "They brought new life to this rock. Some of the animals you see here were brought here by them, kept by them, and in their death these animals have become free to pursue their own life, and their own death. This, you see, is the force that drives us all. With death comes new life."},
    {"choice", "What new life would come here, the very edge of the world?"},
    {"say", "Fengzhi", "There is no edge to the world, only to our knowledge. Whatsoever comes here, by its own volition or not, will leave an indelible mark on this rock."}


}

}