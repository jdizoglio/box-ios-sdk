//
//  BOXAPIQueueManager.h
//  BoxContentSDK
//
//  Created on 2/28/13.
//  Copyright (c) 2013 Box. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BOXAPIOperation, BOXOAuth2Session, BOXAPIOAuth2ToJSONOperation;

/**
 * BOXAPIQueueManager is an abstract class you can use to encapsulate the enqueueing and running
 * of BOXAPIOperation instances. Because this class is abstract, you should not instantiate it
 * directly. You can either use the provided BOXSerialAPIQueueManager or implement your
 * own subclass (see subclassing notes). This class does not enforce its abstractness, but it does
 * not place any operations enqueued via enqueueOperation: into an NSOperationQueue, which means
 * that [BOXAPIOperations](BOXAPIOperation) will not get executed.
 *
 * Subclassing Notes
 * =================
 * Subclasses of BOXAPIQueueManager should override enqueueOperation:. It is important to call
 * this method's super in subclasses because the implementation in BOXAPIQueueManager listens
 * for notifications from the OAuth2Session in order to help implement locking the queue for
 * BOXAPIOAuth2ToJSONOperation instances.
 *
 * Subclasses should ensure BOXAPIOAuth2ToJSONOperation instances are executed with the correct order
 * relative to other BOXAPIOperation instances. Subclasses should ensure that several refresh operations
 * are not executed at once; this has the potential to thrash the shared refresh token and cause a user
 * to become logged out.
 */
@interface BOXAPIQueueManager : NSObject

/**
 * OAuth2Session should be used by subclasses to ensure that multiple
 * refresh operations are not excecuted in parallel.
 *
 * This object is owned by the BoxContentSDK instance.
 */
@property (nonatomic, readwrite, weak) BOXOAuth2Session *OAuth2Session;

/**
 * The set of all currently enqueued or in flight BOXAPIOAuth2ToJSONOperation instances.
 * Subclasses should add these operations as dependencies of other, non-OAuth2 operations.
 *
 * This object is owned by the BoxContentSDK instance.
 */
@property (nonatomic, readwrite, strong) NSMutableSet *enqueuedOAuth2Operations;

/** @name Initializers */

/**
 * Designated initializer
 * @param OAuth2Session This object is needed for locking
 */
- (id)initWithOAuth2Session:(BOXOAuth2Session *)OAuth2Session;

/** @name Enqueue Operations */

/**
 * Set up this instance as an observer for notifications on operation if thew operation is a
 * BOXAPIOAuth2ToJSONOperation instance. Subclasses should enqueue operations received via this
 * method on an NSOperationQueue to be executed.
 *
 * This method synchronizes on OAuth2Session.
 *
 * @param operation The BOXAPIOperation to be enqueued for execution
 */
- (BOOL)enqueueOperation:(BOXAPIOperation *)operation;

/**
 * Add an operation as a dependency to another operation. This method should acquire
 * [BOXAPIOperation APIOperationGlobalLock] and ensure operation is not executing before
 * adding the dependency.
 *
 * @param dependency The operation to add as a dependency.
 * @param operation The operation to add the dependency to.
 *
 * @return YES if dependency was added, NO if the dependency was not added.
 */
- (BOOL)addDependency:(NSOperation *)dependency toOperation:(NSOperation *)operation;

- (void)cancelAllOperations;

@end
