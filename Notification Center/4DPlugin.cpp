/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.c
 #	source generated by 4D Plugin Wizard
 #	Project : Notification Center
 #	author : miyako
 #	2013/07/20
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

#import "listener.h"

UserNotificationListener *listener = nil;

#define Notification_dislay_always 1
#define Notification_system_decides 0

char listenerMode = Notification_system_decides;

void PluginMain(int32_t selector, PA_PluginParameters params)
{
	try
	{
		int32_t pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (int32_t pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{		
		case kInitPlugin:
		case kServerDeinitPlugin:
			listenerLoopFinish();
			break;			
			
		case kDeinitPlugin:
		case kServerInitPlugin:
			break;	
		
// --- Notification Center

		case 1 :
			NOTIFICATION_Get_mode(pResult, pParams);
			break;

		case 2 :
			NOTIFICATION_SET_MODE(pResult, pParams);
			break;

		case 3 :
			NOTIFICATION_SET_METHOD(pResult, pParams);
			break;

		case 4 :
			NOTIFICATION_Get_method(pResult, pParams);
			break;

// --- Notification

		case 5 :
			DELIVER_NOTIFICATION(pResult, pParams);
			break;

		case 6 :
			SCHEDULE_NOTIFICATION(pResult, pParams);
			break;

	}
}

// ------------------------------ Notification Center -----------------------------

#pragma mark -

void NOTIFICATION_Get_mode(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT returnValue;
	returnValue.setIntValue(listenerMode);
	returnValue.setReturn(pResult);
}

void NOTIFICATION_SET_MODE(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT Param1;
	Param1.fromParamAtIndex(pParams, 1);
	listenerMode = Param1.getIntValue();
	
	if(listener){
		if(listenerMode == Notification_dislay_always){
			[listener presentNotification:YES];
		}else{
			[listener presentNotification:NO];				
		}
	}
}

#pragma mark -

void NOTIFICATION_SET_METHOD(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;

	Param1.fromParamAtIndex(pParams, 1);

	NSString *methodName = Param1.copyUTF16String();
	
	if(!Param1.getUTF16Length()){

		listenerLoopFinish();
		
	}else{
	
		int methodId = PA_GetMethodID((PA_Unichar *)Param1.getUTF16StringPtr());
		
		if(methodId){
		
			if(!listener){		
				
				int listenerProcessNumber = PA_NewProcess((void *)listenerLoop, -512*1024, @"$User Notification Listener");
				
				listener = [[UserNotificationListener alloc]
							initWithMethodName:methodName
							methodId:[NSNumber numberWithInt:methodId]
							processNumber:[NSNumber numberWithInt:listenerProcessNumber]];
				
				if(listenerMode == Notification_dislay_always){
					[listener presentNotification:YES];
				}else{
					[listener presentNotification:NO];				
				}
				
				NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
				center.delegate = listener;
				
			}else{
				listener.listenerMethodName = methodName;				
				listener.listenerMethodId = [NSNumber numberWithInt:methodId];				
			}
			
		}
		
	}

}

void NOTIFICATION_Get_method(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;

	if(listener)
		returnValue.setUTF16String(listener.listenerMethodName);

	returnValue.setReturn(pResult);
}

#pragma mark -

// --------------------------------- Notification ---------------------------------

NSUserNotification *createNotification(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT Param3;
	C_TEXT Param4;
	C_TEXT Param5;
	C_TEXT Param6;
	C_TEXT Param7;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);
	Param5.fromParamAtIndex(pParams, 5);
	Param6.fromParamAtIndex(pParams, 6);
	Param7.fromParamAtIndex(pParams, 7);
	
	NSUserNotification *notification = [[NSUserNotification alloc]init];
	
	NSString *title = Param1.copyUTF16String();
	notification.title = title;
	[title release];
	
	NSString *subtitle = Param2.copyUTF16String();
    notification.subtitle = subtitle;
	[subtitle release];
	
	NSString *informativeText = Param3.copyUTF16String();
    notification.informativeText = informativeText;
	[informativeText release];
	
	if(!Param4.getUTF16Length()){
		notification.soundName = nil;
	}else{
		NSString *soundName = Param4.copyUTF16String();
		if([soundName isEqualToString:@"__DEFAULT__"]){
			notification.soundName = NSUserNotificationDefaultSoundName;	
		}else{
			notification.soundName = soundName;		
		}
		[soundName release];
	}	
	
	if(Param5.getUTF16Length()){
		NSString *userInfo = Param5.copyUTF16String();
		NSDictionary *dictionary = PA_createDictionaryFromString(userInfo);
        if(dictionary){
            notification.userInfo = dictionary;
            [dictionary release];
        }
		[userInfo release];
	}	
	
	if(!Param6.getUTF16Length()){
		notification.hasActionButton = NO;	
	}else{
	    notification.hasActionButton = YES;
		NSString *actionButtonTitle = Param6.copyUTF16String();
		notification.actionButtonTitle = actionButtonTitle;
		[actionButtonTitle release];
	}
	
	if(Param7.getUTF16Length()){
		NSString *otherButtonTitle = Param7.copyUTF16String();
		notification.otherButtonTitle = otherButtonTitle;
		[otherButtonTitle release];
	}	
	
	return notification;
}

void DELIVER_NOTIFICATION(sLONG_PTR *pResult, PackagePtr pParams)
{
	NSUserNotification *notification = createNotification(pResult, pParams);
	
	[[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
	
	[notification release];
}

void SCHEDULE_NOTIFICATION(sLONG_PTR *pResult, PackagePtr pParams)
{
	NSUserNotification *notification = createNotification(pResult, pParams);	
	
	C_DATE Param8;
	C_TIME Param9;
	C_TEXT Param10;

	Param8.fromParamAtIndex(pParams, 8);
	Param9.fromParamAtIndex(pParams, 9);
	Param10.fromParamAtIndex(pParams, 10);
		
	NSString *deliveryTimeZone = Param10.copyUTF16String();
	NSTimeZone *timeZone = [NSTimeZone timeZoneWithName:deliveryTimeZone];
    if(!timeZone) timeZone = [NSTimeZone defaultTimeZone];
	
	NSDateComponents *components = [[NSDateComponents alloc]init];
    [components setDay:Param8.getDay()];
    [components setMonth:Param8.getMonth()];
    [components setYear:Param8.getYear()];
	
    [components setSecond:Param9.getSeconds()];
	
	NSCalendar *gregorian = [[NSCalendar alloc]initWithCalendarIdentifier:NSGregorianCalendar];
    notification.deliveryDate = [gregorian dateFromComponents:components];
    [gregorian release];
    [components release];
	
	[[NSUserNotificationCenter defaultUserNotificationCenter] scheduleNotification:notification];

	[notification release];

}

#pragma mark -
	
void listenerLoopFinish(){

	if(listener)
	{
		[listener terminate];
		listener = nil;
	}	

}

void listenerLoopExecuteMethodByID()
{
	PA_Variable	params[6];
	params[0] = PA_CreateVariable(eVK_Unistring);		
	params[1] = PA_CreateVariable(eVK_Unistring);	
	params[2] = PA_CreateVariable(eVK_Unistring);					
	params[3] = PA_CreateVariable(eVK_Unistring);	
	params[4] = PA_CreateVariable(eVK_Unistring);	
	params[5] = PA_CreateVariable(eVK_Unistring);					
	
	PA_Unistring u1 = PA_setUnistringVariable(&params[0], listener.title);
	PA_Unistring u2 = PA_setUnistringVariable(&params[1], listener.subtitle);
	PA_Unistring u3 = PA_setUnistringVariable(&params[2], listener.informativeText);
	PA_Unistring u4 = PA_setUnistringVariable(&params[3], listener.notificationType);				
	PA_Unistring u5 = PA_setUnistringVariable(&params[4], listener.activationType);		
	PA_Unistring u6 = PA_setUnistringVariable(&params[5], listener.userInfo);	
	
	PA_ExecuteMethodByID([listener.listenerMethodId intValue], params, 6);
	
	[listener unlock];
	
	PA_DisposeUnistring(&u1);
	PA_DisposeUnistring(&u2);
	PA_DisposeUnistring(&u3);
	PA_DisposeUnistring(&u4);	
	PA_DisposeUnistring(&u5);
	PA_DisposeUnistring(&u6);
}

void listenerLoop(){
	
	PA_YieldAbsolute();	
	int listenerProcessNumber = PA_GetCurrentProcessNumber();
	PA_FreezeProcess(listenerProcessNumber);
	
	char done = 0;
	
	while (!done)
	{ 
		PA_YieldAbsolute();
		done = (PA_IsProcessDying()) || ([listener shouldTerminate]);
		
		if (!done){		
			
			PA_NewProcess((void *)listenerLoopExecuteMethodByID, 512*1024, listener.notificationType);
			
			PA_FreezeProcess(listenerProcessNumber);
			
		}
	}
	
	listenerLoopFinish();
	
	PA_KillProcess();
}
