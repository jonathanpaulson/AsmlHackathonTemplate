//
//  MasterViewController.h
//  shape-selector
//
//  Created by Jan-Gerd Tenberge on 12.10.18.
//  Copyright © 2018 Jan-Gerd Tenberge. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface MasterViewController : UITableViewController

@property (strong, nonatomic) DetailViewController *detailViewController;


@end

