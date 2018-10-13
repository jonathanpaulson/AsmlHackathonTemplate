//
//  DetailViewController.h
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 12.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Pattern.h"

@interface DetailViewController : UIViewController

@property (strong, nonatomic) Pattern *detailItem;
@property (weak, nonatomic) IBOutlet UIImageView *imageView;

@end

