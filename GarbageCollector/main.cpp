#include <iostream>
#define MAX_STACK_SIZE 512
#define MAX_HEAP_SIZE 512

typedef struct sAllocatedObjects
{
	void* data;
	bool mark;
	struct sAllocatedObjects* next;
} AllocatedObjects;

struct RayVirtualMachine
{
	AllocatedObjects* first_stack_frame;
	AllocatedObjects* first_object;

	AllocatedObjects* stack[MAX_STACK_SIZE];
	AllocatedObjects* heap[MAX_HEAP_SIZE];

	size_t stack_size;
	size_t heap_size;

	size_t max_stack_frames;
	size_t max_objects;
};

AllocatedObjects* create_obj(struct RayVirtualMachine* vm, void* data = NULL)
{
	AllocatedObjects* obj = (AllocatedObjects*)malloc(sizeof(AllocatedObjects));
	obj->data = data;
	obj->mark = 0;
	obj->next = NULL;

	if (vm->first_object)
	{
		AllocatedObjects* current = vm->first_object;
		while (current->next)
			current = current->next;

		current->next = obj;
		return obj;
	}

	vm->first_object = obj;
	return obj;
}

//AllocatedObjects* create_stack_frame(struct RayVirtualMachine* vm, void* data = NULL)
//{}


struct RayVirtualMachine* create_vm()
{
	struct RayVirtualMachine* vm = (struct RayVirtualMachine*)malloc(sizeof(struct RayVirtualMachine*));

	vm->first_object = NULL;
	vm->first_stack_frame = NULL;

	vm->heap_size = 0;
	vm->stack_size = 0;

	vm->max_objects = 8;
	vm->max_stack_frames = 8;

	return vm;
}

void push(struct RayVirtualMachine* vm, AllocatedObjects* obj)
{
	if(vm->stack_size < MAX_STACK_SIZE)
		vm->stack[vm->stack_size++] = obj;
}

AllocatedObjects* pop(struct RayVirtualMachine* vm)
{
	return vm->stack[--vm->stack_size];
}

void add_obj(struct RayVirtualMachine* vm, AllocatedObjects* obj)
{
	if(vm->heap_size < MAX_HEAP_SIZE)
		vm->heap[vm->heap_size++] = obj;
}

void mark(AllocatedObjects* obj)
{
	if (obj->mark) return;
	obj->mark = true;
}

void mark_all(AllocatedObjects** segment, size_t size)
{
	for (size_t i = 0; i < size; i++)
		mark(segment[i]);
}

void collect_heap(struct RayVirtualMachine* vm)
{
	AllocatedObjects** obj = vm->heap;

	while (*obj)
	{
		if (!(*obj)->mark)
		{
			AllocatedObjects* garbage = *obj;
			*obj = garbage->next;
			free(garbage->data);
			free(garbage);
			garbage = NULL;
			--vm->heap_size;
		}
		else
		{
			(*obj)->mark = false;
			obj = &(*obj)->next;
		}
	}
}

void collect_stack(struct RayRayVirtualMachine* vm) {}

void gc(struct RayVirtualMachine* vm)
{
	size_t num_frames = vm->stack_size;
	size_t num_obj = vm->heap_size;

	mark_all(vm->stack, vm->stack_size);
	//mark_all(vm->heap, vm->heap_size);
	
	collect_stack(vm);
	collect_heap(vm);

	vm->max_stack_frames = vm->stack_size * 2;
	vm->max_objects = vm->heap_size * 2;

	//debug
	std::cout << "Collected " << num_frames - vm->stack_size << " frames, left " << vm->stack_size <<
		"\nCollected " << num_obj - vm->heap_size << " objects, left " << vm->heap_size << std::endl;
}

void destroy_vm(struct RayVirtualMachine* vm)
{
	gc(vm);

	vm->heap_size = 0;
	vm->stack_size = 0;

	free(vm);
	vm = NULL;
}

void* AllocHeap(struct RayVirtualMachine* vm, void* data)
{
	if (vm->heap_size > vm->max_objects) gc(vm);
	AllocatedObjects* obj = create_obj(vm, data);
	add_obj(vm, obj);

	return data;
}

void* AllocHeap(struct RayVirtualMachine* vm, size_t size)
{
	return AllocHeap(vm, malloc(size));
}

int main()
{
	RayVirtualMachine* vm = create_vm();
	int* a = (int*)AllocHeap(vm, 4);
	int* b = (int*)AllocHeap(vm, 4);

	//где-то нужно установить null

	destroy_vm(vm);

	system("pause");

	return 0;
}