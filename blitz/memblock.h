// -*- C++ -*-
/***************************************************************************
 * blitz/memblock.h      MemoryBlock<T> and MemoryBlockReference<T>
 *
 * $Id$
 *
 * Copyright (C) 1997-2011 Todd Veldhuizen <tveldhui@acm.org>
 *
 * This file is a part of Blitz.
 *
 * Blitz is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Blitz is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with Blitz.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Suggestions:          blitz-devel@lists.sourceforge.net
 * Bugs:                 blitz-support@lists.sourceforge.net    
 *
 * For more information, please see the Blitz++ Home Page:
 *    https://sourceforge.net/projects/blitz/
 *
 ***************************************************************************/

#ifndef BZ_MEMBLOCK_H
#define BZ_MEMBLOCK_H

#include <blitz/blitz.h>

#include <stddef.h>     // diffType

BZ_NAMESPACE(blitz)

enum preexistingMemoryPolicy { 
  duplicateData, 
  deleteDataWhenDone, 
  neverDeleteData 
};

// This function makes it possible for users to make sure threadsafety
// is enabled.
inline bool isThreadsafe() {
#ifdef BZ_THREADSAFE
  return true;
#else
  return false;
#endif
};


// Forward declaration of MemoryBlockReference
template<typename T_type> class MemoryBlockReference;

// Class MemoryBlock provides a reference-counted block of memory.  This block
// may be referred to by multiple vector, matrix and array objects.  The memory
// is automatically deallocated when the last referring object is destructed.
// MemoryBlock may be subclassed to provide special allocators.
template<typename P_type>
class MemoryBlock {

    friend class MemoryBlockReference<P_type>;

public:
    typedef P_type T_type;

protected:
    // default constructor removed, unused

    explicit MemoryBlock(sizeType items)
    {
        length_ = items;
        allocate(length_);

#ifdef BZ_DEBUG_LOG_ALLOCATIONS
    cout << "MemoryBlock: allocated " << setw(8) << length_ 
         << " at " << ((void *)dataBlockAddress_) << endl;
#endif

        BZASSERT(dataBlockAddress_ != 0);

        references_ = 1;

        BZ_MUTEX_INIT(mutex)
    }

    MemoryBlock(sizeType length, T_type* data)
    {
        length_ = length;
        data_ = data;
        dataBlockAddress_ = data;
        references_ = 1;
        BZ_MUTEX_INIT(mutex)
    }

    virtual ~MemoryBlock()
    {
        if (dataBlockAddress_) 
        {
#ifdef BZ_DEBUG_LOG_ALLOCATIONS
    cout << "MemoryBlock:     freed " << setw(8) << length_
         << " at " << ((void *)dataBlockAddress_) << endl;
#endif

            deallocate();
        }

        BZ_MUTEX_DESTROY(mutex)
    }

    // set mutex locking policy and return true if successful
    bool doLock(bool lockingPolicy) 
    { 
#if defined(BZ_THREADSAFE) && !defined(BZ_THREADSAFE_USE_ATOMIC)
        if (mutexLocking_ == lockingPolicy) { // already set
            return true;
        }
        else if (references_ <= 1) { // no multiple references, safe to change
            mutexLocking_ = lockingPolicy; 
            return true;
        }
        return false; // unsafe to change
#elif defined(BZ_THREADSAFE_USE_ATOMIC)
	// with locking we consider the request successful
	return true; 
#else
	// without threadsafety we return false if user wants to enable locking
	return !lockingPolicy;
#endif
    }

    /* Note that the the MemoryBlock will be created with reference
       count 1, so there is no need to call addReference immediately
       upon creating it. (The creator obviously does have to call
       removeReference, though.) This avoids the initial mutex lock. */
    void          addReference()
    { 
        BZ_MUTEX_LOCK(mutex)
        const int refcount = ++references_; 
	BZ_MUTEX_UNLOCK(mutex)

#ifdef BZ_DEBUG_LOG_REFERENCES
	  cout << "MemoryBlock:    reffed " << setw(8) << length_ 
	       << " at " << ((void *)dataBlockAddress_) << " (r=" 
	       << refcount << ")" << endl;
#endif
    }

    T_type* restrict      data() 
    { 
        return data_; 
    }

    const T_type* restrict data()      const
    { 
        return data_; 
    }

    T_type*&      dataBlockAddress() 
    { 
        return dataBlockAddress_; 
    }

    sizeType        length()    const
    { 
        return length_; 
    }

    int           removeReference()
    {

        BZ_MUTEX_LOCK(mutex)
        const int refcount = --references_;
        BZ_MUTEX_UNLOCK(mutex)

#ifdef BZ_DEBUG_LOG_REFERENCES
	  cout << "MemoryBlock: dereffed  " << setw(8) << length_
	       << " at " << ((void *)dataBlockAddress_) << " (r=" << refcount
	       << ")" << endl;
#endif
        return refcount;
    }

    int references() const
  { 
        BZ_MUTEX_LOCK(mutex)
        const int refcount = references_;
	BZ_MUTEX_UNLOCK(mutex)

        return refcount;
    }

protected:
    inline void allocate(sizeType length);
    void deallocate();

private:   // Disabled member functions
    MemoryBlock(const MemoryBlock<T_type>&)
    { }

    void operator=(const MemoryBlock<T_type>&)
    { }

private:   // Data members
    T_type * restrict     data_;
    T_type *              dataBlockAddress_;
    sizeType              length_;

#if defined(BZ_THREADSAFE) && !defined(BZ_THREADSAFE_USE_ATOMIC)
    // with atomic reference counts, there is no locking
    bool    mutexLocking_;
#endif
    BZ_REFCOUNT_DECLARE(references_)
    BZ_MUTEX_DECLARE(mutex)
};




template<typename P_type>
class MemoryBlockReference {

public:
    typedef P_type T_type;

protected:
    T_type * restrict data_;

private:
    MemoryBlock<T_type>* block_;

public:

    MemoryBlockReference()
    {
        block_ = 0;
	// no block, so nothing to add reference to
        data_ = 0;
    }

    MemoryBlockReference(MemoryBlockReference<T_type>& ref, sizeType offset=0)
    {
        block_ = ref.block_;
        addReference();
        data_ = ref.data_ + offset;
    }

    MemoryBlockReference(sizeType length, T_type* data, 
        preexistingMemoryPolicy deletionPolicy)
    {
        // Create a memory block using already allocated memory. 

        // Note: if the deletionPolicy is duplicateData, this must
        // be handled by the leaf class.  In MemoryBlockReference,
        // this is treated as neverDeleteData; the leaf class (e.g. Array)
        // must duplicate the data.

        if ((deletionPolicy == neverDeleteData) 
            || (deletionPolicy == duplicateData)) {
	    // in this case, we do not need a MemoryBlock to ref-count the data
            block_ = 0;
        }
        else if (deletionPolicy == deleteDataWhenDone) {
            block_ = new MemoryBlock<T_type>(length, data);

#ifdef BZ_DEBUG_LOG_ALLOCATIONS
            cout << "MemoryBlockReference: created MemoryBlock at "
                 << ((void*)block_) << endl;
#endif
        }
	// creating a MemoryBlock automatically sets it to one
	// reference, so we do no longer need to add a reference in
	// the constructor.

        data_ = data;
    }

    explicit MemoryBlockReference(sizeType items)
    {
        block_ = new MemoryBlock<T_type>(items);
	// creating a MemoryBlock automatically sets it to one
	// reference, so we do no longer need to add a reference in
	// the constructor.
        data_ = block_->data();

#ifdef BZ_DEBUG_LOG_ALLOCATIONS
	cout << "MemoryBlockReference: created MemoryBlock at "
	     << ((void*)block_) << endl;
#endif
    }

   ~MemoryBlockReference()
    {
        blockRemoveReference();
    }

protected:
#ifdef BZ_TESTSUITE
public:
#endif
    int numReferences() const
    {
        if (block_) 
            return block_->references();
#ifdef BZ_DEBUG_LOG_REFERENCES
        cout << "Invalid reference count for data at "<< data_ << endl;
#endif
        return -1;      
    }

    bool lockReferenceCount(bool lockingPolicy) const
    {
        if (block_)
            return block_->doLock(lockingPolicy);
        // if we have no block, consider request successful
#ifdef BZ_DEBUG_LOG_REFERENCES
        cout << "No reference count locking for data at "<< data_ << endl;
#endif
        return true;    
    }

    void changeToNullBlock()
    {
        blockRemoveReference();
        block_ = 0;
	// no block, so nothing to add reference to
        data_ = 0;
    }

    void changeBlock(MemoryBlockReference<T_type>& ref, sizeType offset=0)
    {
        blockRemoveReference();
        block_ = ref.block_;
        addReference();
        data_ = ref.data_ + offset;
    }

    void newBlock(sizeType items)
    {
        blockRemoveReference();
        block_ = new MemoryBlock<T_type>(items);
	// creating a memory block automatically sets it to one reference
        data_ = block_->data();

#ifdef BZ_DEBUG_LOG_ALLOCATIONS
	cout << "MemoryBlockReference: created MemoryBlock at "
	     << ((void*)block_) << endl;
#endif
    }

private:
    void blockRemoveReference()
    {
        const int refcount = removeReference();
        if (refcount == 0)
        {
#ifdef BZ_DEBUG_LOG_ALLOCATIONS
	  cout << "MemoryBlock: no more refs, delete MemoryBlock object at "
	       << ((void*)block_) << endl;
#endif

            delete block_;
        }
    }

    void addReference() const 
    {
        if (block_) {
            block_->addReference();
        }
        else {
#ifdef BZ_DEBUG_LOG_REFERENCES
            cout << "Skipping reference count for data at "<< data_ << endl;
#endif
        }
    };

    int removeReference() const 
    {
        if (block_)
            return block_->removeReference();
#ifdef BZ_DEBUG_LOG_REFERENCES
        cout << "Skipping reference count for data at "<< data_ << endl;
#endif
        return -1;      
    };
  
    void operator=(const MemoryBlockReference<T_type>&)
    { }
};


BZ_NAMESPACE_END

#include <blitz/memblock.cc>

#endif // BZ_MEMBLOCK_H
