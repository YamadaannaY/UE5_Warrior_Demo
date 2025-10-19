# UE5_Warrior_Demo
各模块实现功能
    GAS
        GA
------------Base
                AbilityActivationPolicy定义激活策略：1.Reactive 2.OnGiven
                GA下获得CombatComponent ASC等。
                Override: OnGiveAbility（判断激活策略） EndAbility
                NativeApplyEffectSpecHandleToTarget:将GE.SpecHandle应用到目标，并根据GE的赋予关系区分Target和Own

------------Player
                MakeHeroDamageEffectSpecHandle：（1）设置GE的ContextHandle，然后MakeOutgoingSpec实现一个SpecHandle。（2）设置需要SetSetByCallerMagnitude的值，蓝图中动态赋值

------------Enemy
                MakeEnemyDamageEffectSpecHandle

------------C++实现GA:TargetLock

  AT
------------AT_ExecuteOnTask
                实现一个每帧执行的AT，因为UE中并没有让一个能力的部分逻辑每帧执行的功能


  GEE
------------GEExecCauc_DamageTaken
                计算伤害的具体逻辑，绑定GE：SharedDealDamage,在各个会造成伤害的GA中被Apply


  ASCComponent
------------所有需要GA的Actor都会拥有的Component，写入一些共用逻辑。

  AttributeSet
------------PostGameplayEffectExecute 内部自动调用的生命周期函数，当一个GE修改了某个Attribute时调用，将修改的值实时传递给属性，并广播给对应的委托，实现UI显示
                        Attributes各项属性，逻辑最后判断生命值，如果等于0（clamp）则添加DeadTag
                        CachedPawnUIInterface
                            获取UIComponent



  AI
----------BTService
            BTService_OrientToTargetActor，作为一个Service执行，在所有逻辑前进行
            BTTask_RotateToFaceTarget ，作为一个Task执行，作为逻辑进行


  AnimInstances（三层：Base、Character、具体Actor）
----------Base
            实现各大类AnimInstance需要的共同逻辑：（1）寻找Tag作为动画判断条件

----------Character
            所有Character的共同AnimInstance框架
                动画初始化、共有的转换条件变量：加速度、动画朝向

----------Hero
            HeroAnimaInstances
                定义角色动画实例基类，包含动画初始化，状态机各种状态转换条件变量逻辑函数

----------HeroLinkedAnimLayer
                通过MeshComponent获得HeroAnimaInstance



  Character
----------Base
            提供接口实现，为子类提供共有逻辑
                IASC接口获得ASC、ICombatC接口CombatC、IUIC获得UIC
                各基本组件类定义
                DA:StartUp
                PossessBy
                    初始化ASC，检查StartUpData



----------Hero
            重写接口
            组件设置、InputDA配置
            加载StratUpData并赋值于ASC
            绑定IMC，IMC实现BindNativeInput、BindAbilityInput

----------Enemy
            重写接口
            组件配置
            加载StratUpData并赋值于ASC


  Component
----------PawnExtensionComponentBase
            模板函数GetOwingPawn<>()，将GetOwner()向下转换，具有一个static_assert进行安全检查
            模板函数GetOwingController()，获取GetOwingPawn<APawn>()的Controller
            APawn* GetMyOwningPawn() 模板最常用，特别抽离出来称为函数

----------InputComponent
            实现IA与ETriggerEvent、CallBackFunc进行BindAction 的模板函数BindNativeInputAction、BindAbilityInputAction

----------CombatComponent
            Base
                实现一个Map映射（key：Tag  Value：Weapon）
                    OverLappedActors：将碰撞对象录入，只对不在数组中的Actor进行碰撞逻辑。避免对同一对象多次进行伤害
                当前装备武器的Tag：找到当前武器实例，绑定其委托
                RegisterSpawnedWeapon、GetCharacterCarriedWeaponByTag，GetCharacterCurrentEquippedWeapon()
                ToggleWeaponCollision：处理武器碰撞，QueryOnly/NoCollision
                虚函数接收OnComponentBeginOverlap和OnComponentEndOverlap委托广播的回调函数：OnHitTargetActor/OnWeaponPulledFromTargetActor
            Hero
                GetHeroCarriedWeaponByTags/GetHeroCarriedEquippedWeapon()/GetHeroCurrentEquippedWeaponDamageAtLevel
                OnComponentBeginOverlap回调实现：SendGameplayEvent，对具有MeleehHit和HitReact两个Tag的WaitGameplayEvent进行触发
            Enemy
                OnComponentBeginOverlap回调实现：这意味着Enemy攻击触碰到Player，进行两个判断，角色是否格挡或者攻击是否可阻挡。如果通过，则进行角度判断格挡是否成功，如果成功给SuccessfulBlock添加成功格挡                                                 的Tag，触发反击。如果失败则触发MeleeHit,造成伤害并给Player发送HitReact Tag


--------UIComponent
            PawnUIComponent
                单系数多播委托OnCurrentHealthChanged：在属性集中生命值改变则进行广播，蓝图中BindEvent，广播触发Event（改变HealthBar）
            HeroUIComponent
                单系数多播委托(FOnEquippedWeaponChangedDelegate,TSoftObjectPtr<UTexture2D>,SoftWeaponIcon),当武器改变时传入新的软引用图标
                单系数多播委托OnEquippedWeaponChanged，怒气值改变进行广播
            EnemyUIComponent
                TODO

            

  Controller
-----------AI
                GetTeamAttitudeTowards：感知范围内新增对象时进行一次判断，进行TAttitude检查
                EnemyPerceptionComponent：感知系统组件
                UAISenseConfig_Sight：视觉感知配置
                OnEnemyPerceptionUpdated：感知更新自动调用，为黑板字面量TargetActor进行赋值
                BeginPlay
                    配置感知精度：四个SetCrowdAvoidanceQuality的switch-case。
                    设置避让组别
                    设置检测避让的范围


-----------Hero
                构造函数设置TeamID
                GetGenericTeamId()       重写获得TeamID



  DA
----------DA_InputConfig
            将IMC_Default与所有IA及其关联Tag分NativeInputActions和AbilityInputActions进行存储
            FindNativeInputActionByTag:通过Tag遍历NativeInputActions，用于Bind

----------DA_StartUpData
            Base
                分功能存储GA（ActivateOnGivenAbilities[伴生] ReactiveAbilities[被动]）和GE（StartUpGameplayEffects[存储属性数据]）
                GiveToAbilitySystemComponent
                    GrantAbilities将伴生和被动GA->GASpec并传给ASC
                    创建EffectCDO并且ApplyGameplayEffectToSelf
            Hero
                重写GiveToAbilitySystemComponent，在原有Super基础上进行拓展，处理HeroStartUpAbilitySets（Tag与GA映射[属于需要输入触发的GA]）
            Enemy
                重写GiveToAbilitySystemComponent，在原有Super基础上进行拓展，处理EnemyCombatAbilities



  GM
  ------------------------
  
  Interfaces
----------PawnCombatInterface
            IPawnInterface：GetPawnCombatComponent()  统一接口，Actor父类继承

----------PawnUIInterface
            IPawnUIInterface
                 GetPawnUIComponent()纯虚函数，由各子类重写
                GetHeroUIComponent() GetEnemyUIComponent() 常用类型实现避免频繁转换


  Items
----------WarriorProjectileBase
                各组件、DamagePolicy（Hit、Overlap）、GESpecHandle
                OnProjectileHit
                    为BP_OnSpawnProjectileHitFX传入碰撞位置
                    判断格挡
                    格挡成功返回成功Tag，失败HandleApplyProjectileDamage(HitPawn,Data)
                OnProjectileBeginOverlap 暂时未实现
                HandleApplyProjectileDamage：将获得的SpecHandle应用到目标，并给目标添加一个EventTag:HitReact

  Weapon
----------WeaponBase
                    定义一个传递AActor的单参数静态委托，实现两个此委托实例
                    武器各组件
                    监听OnComponentBeginOverlap、OnComponentEndOverlap两个委托的回调函数，回调函数传递HitPawn，执行自定义委托回调，目的是传递OtherActor.
          HeroWeapon
                    HeroWeaponData:封装武器状态下的IMC和GA、还有LinkLayer,Icon等所有Hero武器需要的Data
                    GrantedAbilitySpecHandles:在ASCC中将WeaponGA赋予ASC时，将其封装进一个数组，用这个变量进行存储
                    AssignGrantedAbilitySpecHandles：实现GrantedAbilitySpecHandles存储的函数
                    GetGrantedAbilitySpecHandles()，获得存储数组，用于删除



  WarriorType
            EnumType 枚举类
            StructType 结构类

  Widget
            Base
                NativeOnInitialized()引擎第一次初始化调用，通过UI接口获得HeroUI并调用BP_OnOwingHeroUIComponentInitialized进行一次初始化
                BP_OnOwingHeroUIComponentInitialized：蓝图实现函数，进行UI百分比初始化、Icon加载。
                BP_OnOwingEnemyUIComponentInitialized同样进行初始化操作
                InitEnemyCreatedWidget：封装初始化函数，传入EnemyUIComponent，这个函数在EnemyCharacter的PossessBy中调用


  WarriorGamePlayTags
            记录所有Tag
            WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN Tag声明
            UE_DEFINE_GAMEPLAY_TAG  Tag定义

  WarriorDebugHelper
            信息打印，调试用

  WarriorFunctionLibrary
            常用函数实现



