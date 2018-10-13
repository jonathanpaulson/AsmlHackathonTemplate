//
//  DetailViewController.m
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 12.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import "DetailViewController.h"
#import "BridgeConnector.h"

@interface DetailViewController ()
@property (weak, nonatomic) IBOutlet UISlider *sizeSlider;
@property (weak, nonatomic) IBOutlet UISwitch *colorSwitch;
@end

@implementation DetailViewController

- (void)configureView {
  [[self.imageView layer] setMagnificationFilter:kCAFilterNearest];

  if (self.detailItem) {
    self.imageView.image = self.detailItem.image;
  }
}


- (void)viewDidLoad {
  [super viewDidLoad];
  // Do any additional setup after loading the view, typically from a nib.
  [self configureView];
}


#pragma mark - Managing the detail item

- (void)setDetailItem:(Pattern *)newDetailItem {
  if (_detailItem != newDetailItem) {
      _detailItem = newDetailItem;
      
      // Update the view.
      [self configureView];
  }
}

- (IBAction)setPattern:(id)sender {
  [[BridgeConnector sharedConnector] setPattern:self.detailItem];
}

- (IBAction)patternTouched:(UILongPressGestureRecognizer *)sender {
  CGPoint touchLocation = [sender locationInView:self.imageView];
  uint8_t x = touchLocation.x / self.imageView.bounds.size.width * 32.0;
  uint8_t y = touchLocation.y / self.imageView.bounds.size.height * 32.0;
  uint8_t *raw_data = (uint8_t *)self.detailItem.data.mutableBytes;
  int xr, yr;

  for (int xd = -self.sizeSlider.value; xd <= self.sizeSlider.value; xd++) {
    for (int yd = -self.sizeSlider.value; yd <= self.sizeSlider.value; yd++) {
      if (sqrt(pow(xd, 2.0) + pow(yd, 2.0)) * 2.0 <= self.sizeSlider.value) {
        xr = MIN(31, MAX(0, x + xd));
        yr = MIN(31, MAX(0, y + yd));
        raw_data[yr * 32 + (31 - xr)] = self.colorSwitch.isOn;
      }
    }
  }
  
  self.imageView.image = [self.detailItem image];
}

@end
