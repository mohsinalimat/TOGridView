//
//  TOGridView.h
//  
//  Copyright 2013 Timothy Oliver. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
//  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@class TOGridView;
@class TOGridViewCell;

typedef enum {
    TOGridViewScrollPositionTop=0,
    TOGridViewScrollPositionMiddle,
    TOGridViewScrollPositionBottom
} TOGridViewScrollPosition;

///
/// Data Source Object
///
@protocol TOGridViewDataSource <NSObject>

@required
- (NSUInteger)numberOfCellsInGridView: (TOGridView *)gridView;
- (TOGridViewCell *)gridView: (TOGridView *)gridView cellForIndex: (NSInteger)cellIndex;

@optional
- (BOOL)gridView: (TOGridView *)gridView canMoveCellAtIndex: (NSInteger)cellIndex;
- (BOOL)gridView: (TOGridView *)gridView canEditCellAtIndex:(NSInteger)cellIndex;

@end

///
/// Delegate Object
///
@protocol TOGridViewDelegate <NSObject, UIScrollViewDelegate>

@required
- (CGSize)sizeOfCellsForGridView: (TOGridView *)gridView;
- (NSUInteger)numberOfCellsPerRowForGridView: (TOGridView *)gridView;

@optional
- (CGSize)innerPaddingForGridView: (TOGridView *)gridView;
- (UIView *)gridView: (TOGridView *)gridView decorationViewForRowWithIndex: (NSUInteger)rowIndex;
- (NSUInteger)heightOfRowsInGridView: (TOGridView *)gridView;
- (NSUInteger)offsetOfCellsInRowsInGridView: (TOGridView *)gridView;
- (void)gridView: (TOGridView *)gridView didTapCellAtIndex: (NSUInteger)index;
- (void)gridView: (TOGridView *)gridView didLongTapCellAtIndex: (NSInteger)index;
- (void)gridView: (TOGridView *)gridView didMoveCellAtIndex: (NSInteger)prevIndex toIndex: (NSInteger)newIndex;

@end

@interface TOGridView : UIScrollView <UIGestureRecognizerDelegate> {
    /* The class that is used to spawn cells */
    Class       _cellClass;

    /* The range of cells visible now */
    NSRange     _visibleCellRange;
    
    /* Stores for cells in use, and ones in standby */
    NSMutableSet *_recycledCells;
    NSMutableSet *_visibleCells;

    /* Decoration views */
    NSMutableSet *_recyledDecorationViews;
    NSMutableSet *_visibleDecorationViews;
    
    /* An array of all cells, and whether they're selected or not */
    NSMutableArray *_selectedCells;
    
    /* Padding of cells from edge of view */
    CGSize      _cellPaddingInset;
    /*Size of each cell (This will become the tappable region) */
    CGSize      _cellSize;
    
    /* Number of cells in grid view */
    NSInteger _numberOfCells;
    
    /* Number of cells per row */
    NSInteger _numberOfCellsPerRow;
    
    /* The width between cells on a single row */
    NSInteger _widthBetweenCells;
    
    /* The height of each row (ie the height of each decoration view) */
    NSInteger _rowHeight;
    
    /* Y-position of where the first row starts, after the header */
    NSInteger _offsetFromHeader;
    
    /* Y-offset of cell, within the row */
    NSInteger _offsetOfCellsInRow;

    /* The ImageViews to store the before and after snapshots */
    UIImageView *_beforeSnapshot, *_afterSnapshot;
    
    /* Timer to wait for long presses */
    NSTimer *_longPressTimer;
    
    /* We keep track of the last index that fired a 'longPress' event so we know not to do the 'tapped' event when we press up. */
    NSInteger _longPressIndex;
    
    /* Timer that fires at 60FPS to dynamically animate the scrollView */
    NSTimer *_dragScrollTimer;
    
    /* The amount the offset of the scrollview is incremented on each call of the timer*/
    CGFloat _dragScrollBias;
    
    /* While dragging a cell around, this keeps track of which other cell's area it's currently hovering over */
    NSInteger _cellIndexBeingDraggedOver;
    
    /* The specific cell item that's being dragged by the user */
    TOGridViewCell *_cellBeingDragged;
    
    /* The co-ords of the user's fingers from the last touch event to update the drag cell while it's animating */
    CGPoint _cellDragPoint;
    
    /* The distance between the cell's origin and the user's touch position */
    CGSize _draggedCellOffset;
    
    /* Temporarily halt laying out cells if we need to do something manually that causes iOS to call 'layoutSubViews' */
    __block BOOL _pauseCellLayout;
    
    /* If we need to perform an animation that may trigger the cross-fade animation, temporarily disable it here. */
    __block BOOL _pauseCrossFadeAnimation;
    
    /* Store what protocol methods the delegate/dataSource implement to help reduce overhead involved with checking that at runtime */
    struct {
        unsigned int dataSourceNumberOfCells;
        unsigned int dataSourceCellForIndex;
        unsigned int dataSourceCanMoveCell;
        unsigned int dataSourceCanEditCell;
        
        unsigned int delegateSizeOfCells;
        unsigned int delegateNumberOfCellsPerRow;
        unsigned int delegateInnerPadding;
        unsigned int delegateDecorationView;
        unsigned int delegateHeightOfRows;
        unsigned int delegateOffsetOfCellInRow;
        unsigned int delegateDidTapCell;
        unsigned int delegateDidLongTapCell;
        unsigned int delegateDidMoveCell;
    } _gridViewFlags;
}

@property (nonatomic,assign) id <TOGridViewDataSource>    dataSource;                   /* The object that will provide the grid view with data. */
@property (nonatomic,assign) id <TOGridViewDelegate>      delegate;                     /* The object that the grid view will send events to. */
@property (nonatomic,strong) UIView                       *headerView;                  /* A UIView placed at the top of the grid view */
@property (nonatomic,strong) UIView                       *backgroundView;              /* A UIView placed behind the grid view and locked so it won't scroll */
@property (nonatomic,assign) BOOL                         editing;                      /* Whether the grid view is in an editing state now. */
@property (nonatomic,assign) BOOL                         nonRetinaRenderContexts;      /* If the grid view has a lot of complex cells, setting this can help boost animation performance at a visual expense on Retina devices. */
@property (nonatomic,assign) NSInteger                    dragScrollBoundaryDistance;   /* The distance, in points, from the top of the view downwards that will trigger auto-scrolling when dragging a cell (Same for the bottom). Default is 60 points. */
@property (nonatomic,assign) CGFloat                      dragScrollMaxVelocity;        /* The maximum velocity the view will scroll at when dragging (Ramped up from 0 the closer the finger is to the view boundary). Default is 15 points. */


/* Init the class, and register the cell class to use at the same time. (Else the default TOGridViewCell class is implemented) */
- (id)initWithFrame:(CGRect)frame withCellClass: (Class)cellClass;

/* Register the class that is used to spawn new cell views */
- (void)registerCellClass: (Class)cellClass;

/* Dequeue a recycled cell for reuse */
- (TOGridViewCell *)dequeReusableCell;

/* Dequeue a recycled decoration view for reuse */
- (UIView *)dequeueReusableDecorationView;

/* The range of cells currently visible */
- (NSRange)visibleCells;

/* Add new cells */
- (BOOL)insertCellAtIndex: (NSInteger)index animated: (BOOL)animated;
- (BOOL)insertCellsAtIndices: (NSArray *)indices animated: (BOOL)animated;

/* Delete existing cells */
- (BOOL)deleteCellAtIndex: (NSInteger)index animated: (BOOL)animated;
- (BOOL)deleteCellsAtIndices: (NSArray *)indices animated: (BOOL)animated;

/* Reload existing cells */
- (BOOL)reloadCellAtIndex: (NSInteger)index;
- (BOOL)reloadCellsAtIndices: (NSArray *)indices;

/* Unhighlight a cell after it had been tapped (As opposed to 'deselecting' in edit mode) */
- (void)unhighlightCellAtIndex: (NSInteger)index animated: (BOOL)animated;

/* Reload the entire table */
- (void)reloadGrid;

/* Put the grid view into edit mode (Where cells can be selected and re-ordered.) */
- (void)setEditing:(BOOL)editing animated:(BOOL)animated;   

/* Used to determine the origin (or center) of a cell at a particular index */
- (CGPoint)originOfCellAtIndex: (NSInteger)cellIndex;

/* Used to determine the size of a cell (eg in case specific cells needed to be padded in order to fit) */
- (CGSize)sizeOfCellAtIndex: (NSInteger)cellIndex;

/* Get a list of indices of selected cells */
- (NSArray *)indicesOfSelectedCells;

/* Scroll to a specific cell in the index */
- (void)scrollToCellAtIndex: (NSInteger)cellIndex toPosition: (TOGridViewScrollPosition)position animated: (BOOL)animated completed: (void (^)(void))completed;
@end

/*  
 The old-skool method of declaring classes accepting delegate protocols. Necessary to implement CAAnimationDelegate. 
*/
@interface TOGridView (CAAnimationDelegate)

- (void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag;

@end
