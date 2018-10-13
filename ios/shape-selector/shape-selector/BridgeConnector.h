//
//  BridgeConnector.h
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 13.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Pattern.h"

NS_ASSUME_NONNULL_BEGIN

@interface BridgeConnector : NSObject {
  NSTimer *timer;
}

@property NSString *debugMessages;

+ (instancetype)sharedConnector;
- (void)setPattern:(Pattern *)pattern;

@end

NS_ASSUME_NONNULL_END
