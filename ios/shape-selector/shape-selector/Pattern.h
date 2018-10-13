//
//  Pattern.h
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 12.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface Pattern : NSObject

@property NSString *title;
@property NSMutableData *data;

+ (instancetype)patternFromString:(NSString *)string;
+ (instancetype)randomPattern;
- (UIImage *)image;
- (NSString *)binaryStringForRow:(NSUInteger)row column:(NSUInteger)column;
@end

NS_ASSUME_NONNULL_END
