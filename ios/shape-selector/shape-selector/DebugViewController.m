//
//  DebugViewController.m
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 13.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import "DebugViewController.h"
#import "BridgeConnector.h"

@interface DebugViewController ()

@property (weak, nonatomic) IBOutlet UITextView *textView;

@end

@implementation DebugViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  [[BridgeConnector sharedConnector] addObserver:self
                                      forKeyPath:@"debugMessages" options:NSKeyValueObservingOptionNew
                                         context:nil];
  self.textView.text = @"(No messages retrieved yet)";
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {

  if (object == [BridgeConnector sharedConnector]) {
    dispatch_async(dispatch_get_main_queue(), ^{
      self.textView.text = [BridgeConnector sharedConnector].debugMessages;
    });
  }
  
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
