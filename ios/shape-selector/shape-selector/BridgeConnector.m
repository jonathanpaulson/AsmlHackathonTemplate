//
//  BridgeConnector.m
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 13.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#include <ifaddrs.h>
#include <arpa/inet.h>

#import "BridgeConnector.h"

@implementation BridgeConnector

+ (instancetype)sharedConnector {
  static dispatch_once_t onceToken;
  static BridgeConnector *c = nil;
  dispatch_once(&onceToken, ^{
      c = [[BridgeConnector alloc] init];
  });
  return c;
}

- (void)getDebugData {
  NSString *address = [self bridgeAddress];
  
  if (!address) {
    NSLog(@"No connection for debug data.");
    return;
  }
  
  NSString *full = [NSString stringWithFormat:@"http://%@/debug", address];
  NSURL *requestURL = [NSURL URLWithString:full];
  NSLog(@"Request: %@", requestURL);
  NSURLRequest *urlRequest = [NSURLRequest requestWithURL:requestURL];
  NSOperationQueue *queue = [[NSOperationQueue alloc] init];
  [NSURLConnection sendAsynchronousRequest:urlRequest queue:queue completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
    if (error) {
      NSLog(@"Error,%@", [error localizedDescription]);
    }
    else {
      NSLog(@"Received %lu bytes at %p", (unsigned long)data.length, data.bytes);
      NSString *newMessages = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
      [self setDebugMessages:newMessages];
      NSLog(@"It is now:\n%@", self.debugMessages);
    }
  }];

}

- (instancetype)init {
  self = [super init];
  _debugMessages = @"";
  timer = [NSTimer scheduledTimerWithTimeInterval:5 target:self selector:@selector(getDebugData) userInfo:nil repeats:YES];
  return self;
}

- (NSString *)bridgeAddress {
  NSString *address = NULL;
  struct ifaddrs *interfaces = NULL;
  struct ifaddrs *temp_addr = NULL;
  int success = 0;
  success = getifaddrs(&interfaces);

  if (success == 0) {
    temp_addr = interfaces;

    while(temp_addr != NULL) {

      if(temp_addr->ifa_addr->sa_family == AF_INET) {

        if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
          address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
        }

      }
      
      temp_addr = temp_addr->ifa_next;
    }

  }
  
  // Free memory
  freeifaddrs(interfaces);
  
  if (address) {
    NSCharacterSet *dots = [NSCharacterSet characterSetWithCharactersInString:@"."];
    NSMutableArray *parts = [NSMutableArray arrayWithArray:[address componentsSeparatedByCharactersInSet:dots]];
    parts[3] = @"1";
    address = [parts componentsJoinedByString:@"."];
  }

  return address;
}

- (void)setPattern:(Pattern *)pattern {
  NSString *address = [self bridgeAddress];
  
  if (!address) {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Connection Error"
                                                                             message:@"Could not find the bridge node on the network."
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Ignore"
                                                        style:UIAlertActionStyleCancel
                                                      handler:nil]];
    UIViewController *rootController = [UIApplication sharedApplication].keyWindow.rootViewController;
    [rootController presentViewController:alertController animated:YES completion:NULL];
    return;
  }

  NSOperationQueue *queue = [[NSOperationQueue alloc] init];

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      NSString *encodedPart = [pattern binaryStringForRow:row column:col];
      NSString *full = [NSString stringWithFormat:@"http://%@/pattern/?x=%d&y=%d&p=%d&b=%@", address, col, row, 0, encodedPart];
      NSURL *patternURL = [NSURL URLWithString:full];
      NSLog(@"Sending %@", patternURL);
      NSURLRequest *urlRequest = [NSURLRequest requestWithURL:patternURL];
      [NSURLConnection sendAsynchronousRequest:urlRequest queue:queue completionHandler:^(NSURLResponse *response, NSData *data, NSError *error) {
        if (error) {
          NSLog(@"Error,%@", [error localizedDescription]);
        }
        else {
          NSLog(@"Received %lu bytes at %p", (unsigned long)data.length, data.bytes);
        }
      }];
    }
  }
  
}

@end
