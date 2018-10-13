//
//  Pattern.m
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 12.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import "Pattern.h"

@implementation Pattern

+ (instancetype)patternFromString:(NSString *)string {
  Pattern *pattern = [[Pattern alloc] init];
  uint8_t *raw_data = malloc(32 * 32);
  const char * chars = string.UTF8String;
  for (int i = 0; i < 32 * 32; i++) {
    raw_data[i] = (chars[i] == '1');
  }
  pattern.data = [NSMutableData dataWithBytes:raw_data length:32*32];
  return pattern;
}

+ (instancetype)randomPattern {
  Pattern *pattern = [[Pattern alloc] init];
  uint8_t *raw_data = malloc(32 * 32);
  
  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 32; y++) {
      raw_data[y * 32 + x] = random() % 2;
    }
  }

  pattern.data = [NSMutableData dataWithBytes:raw_data length:32*32];
  free(raw_data);
  return pattern;
}

- (UIImage *)image {
  CGSize imageSize = CGSizeMake(32.0, 32.0);
  UIGraphicsBeginImageContextWithOptions(imageSize, false, 0.0);
  CGContextRef context = UIGraphicsGetCurrentContext();
  uint8_t *raw_data = (uint8_t *)self.data.bytes;
  
  for (int x = 0; x < imageSize.width; x++) {
    for (int y = 0; y < imageSize.height; y++) {
      
      if (raw_data[y * 32 + (31 - x)]) {
        CGContextSetFillColorWithColor(context, [UIColor blackColor].CGColor);
      } else {
        CGContextSetFillColorWithColor(context, [UIColor colorWithWhite:0.0 alpha:0.0].CGColor);
      }
      
      CGContextFillRect(context, CGRectMake(x, y, 1.0, 1.0));
    }
  }
  
  UIImage *image = [UIGraphicsGetImageFromCurrentImageContext() imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
  UIGraphicsEndImageContext();
  return image;
}

- (instancetype)init {
  self = [super init];
  _title = [NSDateFormatter localizedStringFromDate:[NSDate date] dateStyle:NSDateFormatterShortStyle timeStyle:NSDateFormatterMediumStyle];
  return self;
}

- (NSString *)binaryStringForRow:(NSUInteger)row column:(NSUInteger)column {
  NSMutableString *q = [NSMutableString stringWithString:@""];
  uint8_t *raw_data = (uint8_t *)self.data.bytes;
  
  for (int dy = 0; dy < 8; dy++) {
    for (int dx = 0; dx < 8; dx++) {
      NSUInteger x = 8 * column + dx;
      NSUInteger y = 8 * row + dy;
      NSString *a = raw_data[y * 32 + (31 - x)] ? @"1" : @"0";
      [q appendString:a];
    }
  }

  return q;
}

@end
